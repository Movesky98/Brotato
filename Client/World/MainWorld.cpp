#include "MainWorld.h"

#include "Wall.h"
#include "Enemy.h"
#include "Boss.h"
#include "Player.h"
#include "MaterialItem.h"
#include "Projectile.h"

#include "DataManager.h"
#include "ScreenManager.h"

#include "PauseMenu.h"
#include "ShopMenu.h"
#include "PlayerHUD.h"
#include "GameOverScreen.h"
#include "ItemSlot.h"

#include "DataAsset.h"
#include "BossData.h"
#include "CharacterData.h"
#include "EnemyData.h"
#include "WaveData.h"

#include "Component/InventoryComponent.h"

#include "LogManager.h"

#include "World/CameraManager.h"
#include "World/CameraComponent.h"
#include "World/UIManager.h"
#include "World/Input.h"

#include "World/TileMapActor.h"
#include "World/TileMapComponent.h"
#include "World/TileMapRender.h"
#include "World/Tile.h"

#include "Asset/AssetManager.h"
#include "Asset/AnimationManager.h"
#include "Asset/SoundManager.h"

// 테스트용
#include <thread>
#include <chrono>

using namespace std::chrono_literals;


CMainWorld::CMainWorld()
{}

CMainWorld::~CMainWorld()
{

}

bool CMainWorld::Init()
{
	CWorld::Init();
	
	srand((unsigned int)time(0));

	AddAnimations();
	InitializeInputSettings();

	mRemainingWaveTime = mMaxWaveTime;

	auto Player = CreateActor<CPlayer>("Player").lock();
	if (Player)
	{
		mPlayer = Player;
		Player->SetDeathFunction(this, &CMainWorld::OnPlayerDeath);
	}

	mScreenManager = std::make_shared<CScreenManager>();
	auto UIManager = GetUIManager().lock();
	if (UIManager)
	{
		if (!mScreenManager->Init(UIManager, Player)) return false;

		auto ShopScreen = mScreenManager->GetScreen(EScreenType::ShopMenu);
		auto ShopMenu = std::dynamic_pointer_cast<CShopMenu>(ShopScreen.lock());
		if (ShopMenu)
		{
			ShopMenu->SetOnClickedNextWaveFunction(this, &CMainWorld::WaveStart);
		}
	}
	
	SpawnTileMap();
	SpawnWalls();

	return true;
}

void CMainWorld::Update(float DeltaTime)
{
	mInput->Update(DeltaTime);

	if (mPause) return;

	auto iter = mActorList.begin();
	auto iterEnd = mActorList.end();

	for (; iter != iterEnd;)
	{
		if (!iter->second->IsAlive())
		{
			iter = mActorList.erase(iter);
			iterEnd = mActorList.end();
			continue;
		}
		else if (!iter->second->IsEnable())
		{
			++iter;
			continue;
		}

		iter->second->Update(DeltaTime);
		++iter;
	}

	//유효한 카메라들만 남기도록 업데이트
	mCameraManager->Update(DeltaTime);

	mUIManager->Update(DeltaTime);

	if (mStage == EGameplayStage::GameOver) return;
	if (mStage != EGameplayStage::Wave) return;

	mElapsedTime += DeltaTime;
	if (mElapsedTime > 1.f)
	{
		mElapsedTime -= 1.f;
		mRemainingWaveTime -= 1.f;

		mScreenManager->SetWaveCountText((int)mRemainingWaveTime);
		
		if (mRemainingWaveTime < 0.f)
		{
			mMaxWaveTime *= 1.2f;
			mRemainingWaveTime = 0.f;
			WaveEnd();
		}
	}
}

void CMainWorld::AddAnimations()
{
	auto AnimManager = CAssetManager::GetInst()->GetSubManager<CAnimationManager>(EAssetType::Animation2D);
	if (!AnimManager) return;

	/*************************** Slash Animation *****************************/
	AnimManager->CreateAnimation("SlashFrame");
	
	auto Anim = AnimManager->FindAnimation("SlashFrame").lock();
	if (Anim)
	{
		Anim->SetAnimationTextureType(EAnimation2DTextureType::Frame);

		std::vector<std::wstring> Paths;
		for (int i = 1; i <= 6; i++)
		{
			Paths.push_back(TEXT("Custom/Projectile/Slash_") + std::to_wstring(i) + TEXT(".png"));
		}

		std::vector<const TCHAR*> Frames;
		for (const auto& Path : Paths)
		{
			Frames.push_back(Path.c_str());
		}

		Anim->SetTexture("SlashFrame", Frames);
		Anim->AddFrame(7, FVector2(0.f, 0.f), FVector2(1.f, 1.f));
	}

	/*************************** Hit Animation *****************************/
	AnimManager->CreateAnimation("HitFrame");

	Anim = AnimManager->FindAnimation("HitFrame").lock();
	if (Anim)
	{
		Anim->SetAnimationTextureType(EAnimation2DTextureType::Frame);

		std::vector<std::wstring> Paths;
		for (int i = 1; i <= 3; i++)
		{
			Paths.push_back(TEXT("Custom/Hit/Hit_") + std::to_wstring(i) + TEXT(".png"));
		}

		Paths.push_back(TEXT("Custom/Hit/Hit_0.png"));

		std::vector<const TCHAR*> Frames;
		for (const auto& Path : Paths)
		{
			Frames.push_back(Path.c_str());
		}

		Anim->SetTexture("HitFrame", Frames);
		Anim->AddFrame(4, FVector2(0.f, 0.f), FVector2(1.f, 1.f));
	}
}

void CMainWorld::SpawnWalls()
{
	FVector2 TileMapSize;
	TileMapSize.x = mArenaDesc.CountX * mArenaDesc.TileSize;
	TileMapSize.y = mArenaDesc.CountY* mArenaDesc.TileSize;

	int dx[4] = { -1, 1, 0, 0 };
	int dy[4] = { 0, 0, -1, 1 };

	for (int i = 0; i < 4; ++i)
	{
		auto Wall = CreateActor<CWall>("Wall").lock();
		if (Wall)
		{
			Wall->SetWorldPos(TileMapSize.x / 2 * dx[i], TileMapSize.y / 2 * dy[i]);
			// Size는 어떻게?

			FVector2 BoxSize;
			BoxSize.x = dx[i] ? 192.f : TileMapSize.x;
			BoxSize.y = dy[i] ? 192.f : TileMapSize.y;
			Wall->SetSize(BoxSize);

			mWalls.push_back(Wall);
		}
	}
}

void CMainWorld::SpawnTileMap()
{
	auto TileMap = CreateActor<CTileMapActor>("TileMap").lock();
	if (TileMap)
	{
		auto TileRender = TileMap->GetRender().lock();
		auto TileMapComp = TileMap->GetTileMapComp().lock();
		if (TileMapComp && TileRender)
		{
			TileMapComp->CreateTile(ETileShape::Rect, mArenaDesc.CountX, mArenaDesc.CountY, FVector2(mArenaDesc.TileSize, mArenaDesc.TileSize), -1, false);

			TileMapComp->SetTileTexture(ETileTextureType::Back, "BackTexture", TEXT("Custom/Tile/Background_Gradient_01.png"));
			TileMapComp->SetTileTexture(ETileTextureType::Tile, "TileTexture", TEXT("Custom/Tile/TilesWithOutline_01.png"));

			// 타일 텍스쳐의 프레임 지정 (0 ~ 11)
			for (int i = 0; i < 3; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					TileMapComp->AddTileFrame(
						i * 64.f,
						j * 64.f,
						64.f + i * 64.f,
						64.f + j * 64.f);
				}
			}

			// 타일 테두리 텍스쳐의 프레임 지정
			// 북/남/서/동 형태로 각각 6개씩 (12 ~ 35)
			for(int i = 4; i < 8; ++i)
			{
				for (int j = 0; j < 6; ++j)
				{
					TileMapComp->AddTileFrame(
						j * 64.f,
						i * 64.f,
						64.f + j * 64.f,
						64.f + i * 64.f);
				}
			}

			// 타일 테두리 텍스쳐의 프레임 지정
			// LT/RT/LB/RB 순서 (36 ~ 39)
			for (int i = 6; i < 8; ++i)
			{
				for (int j = 4; j < 6; ++j)
				{
					TileMapComp->AddTileFrame(
						i * 64.f,
						j * 64.f,
						64.f + i * 64.f,
						64.f + j * 64.f);
				}
			}

			TileRender->EnableTileAlphaBlend();

			// 타일을 그리는 건 TileMapComp::SetTileFrame() 인 듯.
			int PlainFrameWeight = 50;
			int TotalTileFrame = PlainFrameWeight + mArenaDesc.TileFrameCount;
			int TotalTileCount = mArenaDesc.CountX * mArenaDesc.CountY;
			for (int i = 0; i < TotalTileCount; ++i)
			{
				int Y = i / mArenaDesc.CountX;
				int X = i % mArenaDesc.CountX;

				if (X == 0 || X == mArenaDesc.CountX - 1 || Y == 0 || Y == mArenaDesc.CountY - 1)
					continue;

				int Rand = rand() % TotalTileFrame;
				int Frame = Rand < PlainFrameWeight ? mArenaDesc.PlainTileFrame : Rand - PlainFrameWeight;

				TileMapComp->SetTileFrame(i, Frame);
				TileMapComp->GetTileList();
			}

			RenderOutlineTiles(TileMapComp);
		}

		FVector2 TotalTileMapSize;
		TotalTileMapSize.x = mArenaDesc.CountX * mArenaDesc.TileSize;
		TotalTileMapSize.y = mArenaDesc.CountY * mArenaDesc.TileSize;

		TotalTileMapSize /= 2.f;
		TotalTileMapSize *= -1.f;

		TileMap->SetWorldPos(TotalTileMapSize);
	}
}

void CMainWorld::RenderOutlineTiles(std::shared_ptr<class CTileMapComponent> TileMapComp)
{
	// 동, 서, 남, 북
	int dx[4] = { 1, -1, 0, 0 };
	int dy[4] = { 0 , 0, -1, 1 };

	int TotalTileCount = mArenaDesc.CountX * mArenaDesc.CountY;

	// 테두리 타일에 해당하는 인덱스를 저장
	std::unordered_multimap<int/* Index */, int /* NeighborDireciton */> OutlineTileIndices;
	const auto& TileList = TileMapComp->GetTileList();

	for (int i = 0; i < TotalTileCount; ++i)
	{
		int Y = i / mArenaDesc.CountX;
		int X = i % mArenaDesc.CountX;
		
		if (X == 0 || X == mArenaDesc.CountX - 1 || Y == 0 || Y == mArenaDesc.CountY - 1)
			continue;

		int Index = 0;
		while (Index < 4)
		{
			int NewX = dx[Index] + X;
			int NewY = dy[Index] + Y;

			if (!TileList[NewX + NewY * mArenaDesc.CountX]->IsRender())
			{
				OutlineTileIndices.insert(std::pair(i, Index));
			}
			
			++Index;
		}
	}
	
	enum
	{
		DIR_EAST = 1 << 0,
		DIR_WEST = 1 << 1,
		DIR_SOUTH = 1 << 2,
		DIR_NORTH = 1 << 3,
	};

	auto iter = OutlineTileIndices.begin();
	auto iterEnd = OutlineTileIndices.end();

	for (; iter != iterEnd; ++iter)
	{
		const int TileIndex = iter->first;

		auto [Begin, End] = OutlineTileIndices.equal_range(TileIndex);
		int DirMask = 0;
		for (auto it = Begin; it != End; ++it)
		{
			DirMask |= 1 << it->second;
		}

		int Frame = 0;

		switch (DirMask)
		{
		case DIR_SOUTH:
			/* 12 : 타일 종류 개수, */
			Frame = 12 + rand() % 6;
			break;
		case DIR_NORTH:
			Frame = 18 + rand() % 6;
			break;
		case DIR_WEST:
			Frame = 24 + rand() % 6;
			break;
		case DIR_EAST:
			Frame = 30 + rand() % 6;
			break;
		case DIR_NORTH | DIR_WEST:
			Frame = 37;
			break;
		case DIR_SOUTH | DIR_WEST:
			Frame = 36;
			break;
		case DIR_NORTH | DIR_EAST:
			Frame = 39;
			break;
		case DIR_SOUTH | DIR_EAST:
			Frame = 38;
			break;

		default:
			break;
		}

		TileMapComp->SetTileFrame(TileIndex, Frame);
	}
}

void CMainWorld::InitializeInputSettings()
{
	if (mInput)
	{
		mInput->AddBindKey("TogglePauseMenu", VK_ESCAPE);
		mInput->SetBindFunction("TogglePauseMenu", EInputType::Press, this, &CMainWorld::TogglePauseMenu);
	}
}

void CMainWorld::OnEnemyDeath(const FVector3& DeathPos, bool IsCountedSpawn)
{
	if (mStage != EGameplayStage::Wave) return;

	SpawnMaterial(DeathPos);

	if (!IsCountedSpawn) return;
	AliveEnemyCount--;

	SpawnWaveEnemies();
}

void CMainWorld::SpawnWaveEnemies()
{
	if (!mWaveData) return;

	const std::vector<FWaveSpawnDesc>& Spawns = mWaveData->Spawns;

	// 현재 항목이 들어갈 자리가 없으면 뒤 항목도 기다린다.
	while (MaxAliveEnemy - AliveEnemyCount >= Spawns[mSpawnIndex].Count)
	{
		const FWaveSpawnDesc& Spawn = Spawns[mSpawnIndex];
		for (int i = 0; i < Spawn.Count; i++)
		{
			CreateSpawnPoint(ESpawnType::Enemy, Spawn.EnemyID, ESpawnSource::Wave);
		}

		mSpawnIndex = (mSpawnIndex + 1) % (int)Spawns.size();
	}
}

bool CMainWorld::IsCountedSpawn(ESpawnType Type, ESpawnSource Source) const
{
	return Source == ESpawnSource::Wave && Type == ESpawnType::Enemy;
}

void CMainWorld::OnPlayerDeath()
{
	mScreenManager->ShowScreen(EScreenType::GameOverScreen);
	mStage = EGameplayStage::GameOver;

	ClearObjects<CEnemy>();
	ClearObjects<CSpawnPoint>();
	ClearObjects<CProjectile>();
}

void CMainWorld::CreateSpawnPoint(ESpawnType Type, int ID, ESpawnSource Source)
{
	auto SpawnPoint = CreateActor<CSpawnPoint>("SpawnPoint").lock();
	if (!SpawnPoint) return;

	std::weak_ptr<CDataAsset> Data;
	switch (Type)
	{
	case ESpawnType::Enemy:
		Data = CDataManager::GetInst()->GetEnemyData(ID);
		break;
	case ESpawnType::Structure:
		break;
	case ESpawnType::Boss:
		Data = CDataManager::GetInst()->GetBossData(ID);
		break;
	default:
		break;
	}

	FSpawnDesc Desc;
	Desc.Data = Data;
	Desc.Source = Source;
	Desc.Type = Type;

	SpawnPoint->SetWorldPos(GetRandomSpawnPosition());
	SpawnPoint->InitializeSpawnSettings(Desc);
	SpawnPoint->SetOnSpawnEntityFunction(this, &CMainWorld::SpawnEntity);

	// 스폰 대기 중인 적도 제한 수에 포함한다.
	if (IsCountedSpawn(Type, Source))
		AliveEnemyCount++;
}

void CMainWorld::SpawnEntity(const FVector3 SpawnPos, FSpawnDesc Desc)
{
	if (Desc.Data.expired()) return;
	if (mStage != EGameplayStage::Wave) return;

	bool IsCounted = IsCountedSpawn(Desc.Type, Desc.Source);

	switch (Desc.Type)
	{
	case ESpawnType::Enemy:
		SpawnEnemy(SpawnPos, mPlayer, Desc.Data.lock(), IsCounted);
		break;
	case ESpawnType::Structure:
		break;
	case ESpawnType::Boss:
		SpawnBoss(SpawnPos, mPlayer, Desc.Data.lock(), IsCounted);
		break;
	default:
		break;
	}
}

void CMainWorld::SpawnPlayer()
{
	auto Player = mPlayer.lock();
	if (Player)
	{
		Player->ActiveForWave();
	}
}

void CMainWorld::SpawnEnemy(const FVector3 SpawnPos, const std::weak_ptr<class CPlayer>& wPlayer, const std::shared_ptr<class CDataAsset> Data, bool IsCountedSpawn)
{
	if (!Data || wPlayer.expired()) return;

	auto Player = wPlayer.lock();
	auto Enemy = CreateActor<CEnemy>("Enemy").lock();

	if (Player && Enemy)
	{
		Enemy->InitializeProperties(Data, Player, IsCountedSpawn);
		Enemy->SetWorldPos(SpawnPos);
		Enemy->SetNotifyDeathFunc(this, &CMainWorld::OnEnemyDeath);
	}
}

void CMainWorld::SpawnBoss(const FVector3 SpawnPos, const std::weak_ptr<class CPlayer>& wPlayer, const std::shared_ptr<class CDataAsset> Data, bool IsCountedSpawn)
{
	if (!Data || wPlayer.expired()) return;

	auto Player = wPlayer.lock();
	auto Boss = CreateActor<CBoss>("Boss").lock();

	if (Player && Boss)
	{
		Boss->InitializeProperties(Data, Player, IsCountedSpawn);
		Boss->SetWorldPos(SpawnPos);
		Boss->SetNotifyDeathFunc(this, &CMainWorld::OnEnemyDeath);
	}
}

void CMainWorld::SpawnMaterial(const FVector3& SpawnPos)
{
	auto Material = CreateActor<CMaterialItem>("MaterialItem").lock();

	if (Material)
	{
		Material->SetWorldPos(SpawnPos);
	}
}

void CMainWorld::WaveStart()
{
	mStage = EGameplayStage::Wave;
	mCurrentWave++;
	mRemainingWaveTime = mMaxWaveTime;

	mScreenManager->HideScreen();
	mScreenManager->ShowPlayerHUD(mCurrentWave, (int)mRemainingWaveTime);

	SpawnPlayer();

	// 누적하면 소수점이 잘려 쌓이므로 기준 값에서 매번 계산한다.
	MaxAliveEnemy = (int)(BaseMaxAliveEnemy * std::pow(1.2f, (float)(mCurrentWave - 1)));

	mWaveData = CDataManager::GetInst()->GetWaveData(mCurrentWave);
	mSpawnIndex = 0;

	if (!mWaveData) return;

	// 보스는 웨이브 시작 시 한 번만 스폰하고 제한 수에서 제외된다.
	if (mWaveData->Boss != 0)
	{
		CreateSpawnPoint(ESpawnType::Boss, mWaveData->Boss, ESpawnSource::Wave);
	}

	SpawnWaveEnemies();
}

void CMainWorld::WaveEnd()
{
	// 플레이어 사망 여부에 따라 Shop으로 갈지 GameOver로 갈지 여기서 결정하면 될 듯.
	// 그리고 웨이브 끝나면 살아있던 Enemy 다 죽어야 함

	// 다음 웨이브 데이터가 없으면 마지막 웨이브였으므로 상점 대신 게임 오버.
	bool IsLastWave = CDataManager::GetInst()->GetWaveData(mCurrentWave + 1) == nullptr;
	mStage = IsLastWave ? EGameplayStage::GameOver : EGameplayStage::Shop;

	// WaveEnd Delegate 함수 실행.
	// 적, 스폰 포인트 객체는 여기에 오면 안됨.
	// 차라리 여기서 삭제하게 하는게 나을 수도 있긴 하겠다.
	auto iter = mOnWaveEndedFuncs.begin();
	auto iterEnd = mOnWaveEndedFuncs.end();
	for (; iter != iterEnd;)
	{
		if (iter->first.expired())
		{
			iter = mOnWaveEndedFuncs.erase(iter);
			iterEnd = mOnWaveEndedFuncs.end();
			continue;
		}

		if (iter->second)
			iter->second();

		iter++;
	}

	// 생존해있는 적들 삭제
	ClearObjects<CEnemy>();
	ClearObjects<CSpawnPoint>();
	ClearObjects<CProjectile>();

	auto Player = mPlayer.lock();
	if (Player)
	{
		Player->DeactiveForWave();
	}

	// 플레이어가 죽었으면 GG
	
	// 얻은 상자, 레벨 업 여부 확인 해야 함.
	AliveEnemyCount = 0;

	if (IsLastWave)
	{
		mScreenManager->ShowScreen(EScreenType::GameOverScreen);
		return;
	}

	EnterShop();
}

void CMainWorld::EnterShop()
{
	// Show Shop UI
	mScreenManager->ShowScreen(EScreenType::ShopMenu);

	// Hide PlayerHUD
	mScreenManager->HidePlayerHUD();
}

FVector3 CMainWorld::GetRandomSpawnPosition() const
{
	FVector2 TileMapSize;
	TileMapSize.x = mArenaDesc.CountX * mArenaDesc.TileSize;
	TileMapSize.y = mArenaDesc.CountY * mArenaDesc.TileSize;

	FVector3 SpawnPos;
	SpawnPos.x = (float)(rand() % (int)(TileMapSize.x / 2 - 2 * mArenaDesc.TileSize));
	SpawnPos.y = (float)(rand() % (int)(TileMapSize.y / 2 - 2 * mArenaDesc.TileSize));

	// Negative 여부
	SpawnPos.x *= rand() % 2 ? -1.f : 1.f;
	SpawnPos.y *= rand() % 2 ? -1.f : 1.f;

	return SpawnPos;
}

std::weak_ptr<class CPlayer> CMainWorld::GetPlayer() const
{
	return mPlayer;
}

void CMainWorld::InitializeWorldSetting(std::weak_ptr<CDataAsset> Data)
{
	if (Data.expired()) return;

	auto CharacterData = std::dynamic_pointer_cast<CCharacterData>(Data.lock());
	if (CharacterData)
	{
		mPlayerData = CharacterData;

		auto Player = mPlayer.lock();
		if (Player)
		{
			Player->SetPlayerDesc(CharacterData);
		}
	}

	WaveStart();
}

void CMainWorld::TogglePauseMenu()
{
	EScreenType CurScreenType = mScreenManager->GetCurrentScreenType();

	if (CurScreenType == EScreenType::PauseMenu)
	{
		mScreenManager->HideScreen();
		mPause = false;
	}
	else
	{
		if (mScreenManager->ShowScreen(EScreenType::PauseMenu))
			mPause = true;
	}
}