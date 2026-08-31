#include "MainWorld.h"

#include "Wall.h"
#include "Enemy.h"
#include "Player.h"
#include "MaterialItem.h"
#include "DataManager.h"

#include "PauseMenu.h"
#include "ShopMenu.h"
#include "PlayerHUD.h"
#include "ItemSlot.h"

#include "EnemyData.h"
#include "BackgroundActor.h"

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
	CDataManager::DestroyInst();
}

bool CMainWorld::Init()
{
	CWorld::Init();
	
	srand((unsigned int)time(0));
	
	CDataManager::GetInst()->InitializeCSVData();
	InitializeInputSettings();

	mRemainingWaveTime = mMaxWaveTime;

	auto Player = CreateActor<CPlayer>("Player").lock();
	if (Player)
	{
		mPlayer = Player;
	}

	auto UIManager = GetUIManager().lock();
	if (UIManager)
	{
		mPlayerHUD = UIManager->CreateWidgetContainer<CPlayerHUD>("PlayerHUD");
		auto PlayerHUD = mPlayerHUD.lock();
		auto Player = mPlayer.lock();
		if (PlayerHUD)
		{
			PlayerHUD->SetPos(0.f, 0.f);
			PlayerHUD->SetEnable(true);

			PlayerHUD->SetLifeText(Player->GetHP(), Player->GetMaxHP());
			Player->SetUpdateHPFunction(PlayerHUD.get(), &CPlayerHUD::SetLifeText);
		}

		mPauseMenu = UIManager->CreateWidgetContainer<CPauseMenu>("PauseMenu", 0);
		auto PauseMenu = mPauseMenu.lock();
		if (PauseMenu)
		{
			PauseMenu->SetPos(0.f, 0.f);
			PauseMenu->SetEnable(false);
		}

		mShopMenu = UIManager->CreateWidgetContainer<CShopMenu>("ShopMenu", 0);
		auto ShopMenu = mShopMenu.lock();
		if (ShopMenu)
		{
			ShopMenu->SetPos(0.f, 0.f);
			ShopMenu->SetEnable(false);
			ShopMenu->InitializeWithInventory(Player->GetInventoryComponent());
			ShopMenu->SetOnClickedNextWaveFunction(this, &CMainWorld::WaveStart);
		}
	}
	
	SpawnTileMap();
	SpawnWalls();
	WaveStart();

	return true;
}

void CMainWorld::Update(float DeltaTime)
{
	CWorld::Update(DeltaTime);

	if (mStage != EGameplayStage::Wave) return;

	mElapsedTime += DeltaTime;
	if (mElapsedTime > 1.f)
	{
		mElapsedTime -= 1.f;
		mRemainingWaveTime -= 1.f;

		auto PlayerHUD = mPlayerHUD.lock();
		if (PlayerHUD)
		{
			PlayerHUD->SetWaveCountText((int)mRemainingWaveTime);
		}
		
		if (mRemainingWaveTime < 0.f)
		{
			mMaxWaveTime *= 1.2f;
			mRemainingWaveTime = 0.f;
			WaveEnd();
		}
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

void CMainWorld::OnEnemyDeath(const FVector3& DeathPos)
{
	if (mStage != EGameplayStage::Wave) return;

	SpawnMaterial(DeathPos);
	AliveEnemyCount--;

	if (MaxAliveEnemy > AliveEnemyCount)
	{
		SpawnEnemy(mPlayer);
	}
}

void CMainWorld::SpawnPlayer()
{
	auto Player = mPlayer.lock();
	if (Player)
	{
		Player->SetEnable(true);
		Player->SetWorldPos(0.f, 0.f);
		Player->SetHP(Player->GetMaxHP());
		Player->SpawnWeapons();
	}
}

void CMainWorld::SpawnEnemy(const std::weak_ptr<class CPlayer>& wPlayer)
{
	FVector2 TileMapSize;
	TileMapSize.x = mArenaDesc.CountX * mArenaDesc.TileSize;
	TileMapSize.y = mArenaDesc.CountY * mArenaDesc.TileSize;

	FVector3 SpawnPos;
	SpawnPos.x = (float)(rand() % (int)(TileMapSize.x / 2 - 2 * mArenaDesc.TileSize));
	SpawnPos.y = (float)(rand() % (int)(TileMapSize.y / 2 - 2 * mArenaDesc.TileSize));

	auto Enemy = CreateActor<CEnemy>("Enemy").lock();
	auto Player = wPlayer.lock();
	if (Enemy && Player)
	{
		auto EnemyInfo = CDataManager::GetInst()->GetRandomEnemyData();
		// auto EnemyInfo = CDataManager::GetInst()->GetEnemyData(1001);
		Enemy->InitializeEnemyInfo(EnemyInfo, mPlayer);

		bool IsNegativeX = rand() % 2;
		bool IsNegativeY = rand() % 2;

		// Negative 여부
		SpawnPos.x *= IsNegativeX ? -1.f : 1.f;
		SpawnPos.y *= IsNegativeY ? -1.f : 1.f;

		Enemy->SetWorldPos(SpawnPos);
		Enemy->SetNotifyDeathFunc(this, &CMainWorld::OnEnemyDeath);

		AliveEnemyCount++;
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

	auto ShopMenu = mShopMenu.lock();
	if(ShopMenu)
	{
		ShopMenu->SetEnable(false);
	}

	auto PlayerHUD = mPlayerHUD.lock();
	if (PlayerHUD)
	{
		PlayerHUD->SetEnable(true);
		PlayerHUD->SetWaveText(mCurrentWave);

		mRemainingWaveTime = mMaxWaveTime;
		PlayerHUD->SetWaveCountText((int)mRemainingWaveTime);
	}

	SpawnPlayer();

	for (int i = 0; i < MaxAliveEnemy; i++)
	{
		SpawnEnemy(mPlayer);
	}
}

void CMainWorld::WaveEnd()
{
	// 플레이어 사망 여부에 따라 Shop으로 갈지 GameOver로 갈지 여기서 결정하면 될 듯.
	// 그리고 웨이브 끝나면 살아있던 Enemy 다 죽어야 함
	mStage = EGameplayStage::Shop;

	// 생존해있는 적들 삭제
	std::list<std::weak_ptr<CEnemy>> Enemies;

	if (FindObjectList<CEnemy>(Enemies))
	{
		for (auto& EnemyPtr : Enemies)
		{
			auto Enemy = EnemyPtr.lock();
			if (Enemy)
			{
				// 우선 이렇게
				Enemy->Destroy();
			}
		}
	}

	auto Player = mPlayer.lock();
	if (Player)
	{
		Player->DestroyWeapons();
		Player->SetEnable(false);
	}

	// 플레이어가 죽었으면 GG
	
	// 얻은 상자, 레벨 업 여부 확인 해야 함.
	AliveEnemyCount = 0;
	EnterShop();
}

void CMainWorld::EnterShop()
{
	// Show Shop UI
	auto ShopMenu = mShopMenu.lock();
	if (ShopMenu)
	{
		ShopMenu->Show(mCurrentWave);	
	}

	// Hide PlayerHUD
	auto PlayerHUD = mPlayerHUD.lock();
	if (PlayerHUD)
	{
		PlayerHUD->SetEnable(false);
	}
	

	// WaveStart();
}

void CMainWorld::TogglePauseMenu()
{
	auto PauseMenu = mPauseMenu.lock();
	auto Player = mPlayer.lock();
	if (PauseMenu && Player)
	{
		if (PauseMenu->IsEnable())
		{
			PauseMenu->SetEnable(false);
		}
		else
		{
			PauseMenu->UpdateInventory(Player->GetInventoryComponent());
			PauseMenu->SetEnable(true);
		}
	}
}