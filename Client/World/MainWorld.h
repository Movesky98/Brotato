#pragma once

#include "World/World.h"
#include "SpawnPoint.h"

enum class EGameplayStage
{
	None = -1,
	Wave,
	Shop,
	GameOver,
	MAX
};

struct FArenaDesc
{
	int CountX = 32;
	int CountY = 24;
	float TileSize = 64.f;
	int PlainTileFrame = 11;	// 맨바닥 프레임 위치
	int TileFrameCount = 12;	// 타일의 프레임 총 개수
};

class CMainWorld :
	public CWorld
{
public:
	CMainWorld();
	virtual ~CMainWorld();

public:
	virtual bool Init();
	virtual void Update(float DeltaTime) override;

protected:
	std::weak_ptr<class CPlayer> mPlayer;
	std::weak_ptr<class CPauseMenu> mPauseMenu;
	std::weak_ptr<class CShopMenu> mShopMenu;
	std::weak_ptr<class CPlayerHUD> mPlayerHUD;
	std::weak_ptr<class CGameOverScreen> mGameOverScreen;
	std::vector<std::weak_ptr<class CWall>> mWalls;

	std::vector<std::pair<std::weak_ptr<CActor>, std::function<void()>>> mOnWaveEndedFuncs;

	std::shared_ptr<class CScreenManager> mScreenManager;

public:
	template<typename T>
	void SetOnWaveEndedFunction(T* Object, void(T::* Func)())
	{
		std::weak_ptr<CActor> BindObject = Object->GetThisPtr<CActor>();
		auto BindFunc = std::bind(Func, Object);

		mOnWaveEndedFuncs.push_back(std::make_pair(BindObject, BindFunc));
	}

	template<typename T>
	void RemoveOnWaveEndedFunction(T* Object)
	{
		std::weak_ptr<CActor> ObjectPtr = Object->GetThisPtr<CActor>();
		
		auto iter = mOnWaveEndedFuncs.begin();
		auto iterEnd = mOnWaveEndedFuncs.end();
		
		for (; iter != iterEnd;)
		{
			if (iter->first.owner_before(ObjectPtr) && ObjectPtr.owner_before(iter->first))
			{
				mOnWaveEndedFuncs.erase(iter);
				return;
			}

			++iter;
		}
	}

protected:
	void InitializeInputSettings();
	
	/* TileMap */
protected:
	void SpawnWalls();
	void SpawnTileMap();

	// 전체 타일맵을 그린 후 테두리 타일을 그리는 함수
	void RenderOutlineTiles(std::shared_ptr<class CTileMapComponent> TileMapComp);


	/* Gameplay Logic */
protected:
	int MaxAliveEnemy = 60;				// 동시 생성 가능한 적 최대 수
	int AliveEnemyCount = 0;			// 현재 살아있는 적 수

	float mMaxWaveTime = 10.f;			// 최대 웨이브 시간 
	float mElapsedTime = 0.f;
	float mRemainingWaveTime = 10.f;	// 현재 웨이브가 끝날 때까지 남아있는 시간

	int mCurrentWave = 0;
	int mCurrentMaterial = 0;

	EGameplayStage mStage = EGameplayStage::None;

	FArenaDesc mArenaDesc;

	void CreateSpawnPoint(ESpawnType Type);
	void SpawnEntity(const FVector3 SpawnPos, ESpawnType Type, std::weak_ptr<class CDataAsset> Data);

	void SpawnPlayer();
	
	void SpawnEnemy(const FVector3 SpawnPos, const std::weak_ptr<class CPlayer>& wPlayer, const std::shared_ptr<class CEnemyData> Data);
	void SpawnEnemy(const std::weak_ptr<class CPlayer>& wPlayer);
	void OnEnemyDeath(const FVector3& DeathPos);
	void OnPlayerDeath();
	void SpawnMaterial(const FVector3& SpawnPos);
	
	template<typename T> 
	void ClearObjects()
	{
		std::list<std::weak_ptr<T>> List;
		FindObjectList<T>(List);
		for (auto& ElementWeak : List)
		{
			auto Element = ElementWeak.lock();
			if (Element)
			{
				Element->Destroy();
			}
		}
	}


	void WaveStart();
	void WaveEnd();
	void EnterShop();

public:
	const FArenaDesc GetArenaDesc() const { return mArenaDesc; }
	FVector3 GetRandomSpawnPosition() const;
	std::weak_ptr<class CPlayer> GetPlayer() const;

	int GetCurrentWave() const { return mCurrentWave; }
	EGameplayStage GetStage() const { return mStage; }
	
	/* UI */
protected:
	void TogglePauseMenu();
};

