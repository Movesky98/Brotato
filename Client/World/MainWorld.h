#pragma once

#include "World/World.h"

enum class EGameplayStage
{
	None = -1,
	Wave,
	Shop,
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

protected:
	std::weak_ptr<class CPlayer> mPlayer;
	std::weak_ptr<class CPauseMenu> mPauseMenu;
	std::weak_ptr<class CShopMenu> mShopMenu;
	std::weak_ptr<class CPlayerHUD> mPlayerHUD;
	std::vector<std::weak_ptr<class CWall>> mWalls;

protected:
	void InitializeInputSettings();

public:
	virtual bool Init();
	virtual void Update(float DeltaTime) override;
	
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

	EGameplayStage mStage = EGameplayStage::None;

	FArenaDesc mArenaDesc;

	void SpawnPlayer();
	void SpawnEnemy(const std::weak_ptr<class CPlayer>& wPlayer);
	void OnEnemyDeath(const FVector3& DeathPos);
	void SpawnMaterial(const FVector3& SpawnPos);

	void WaveStart();
	void WaveEnd();

	void EnterShop();

public:

	const FArenaDesc GetArenaDesc() const { return mArenaDesc; }

	/* UI */
protected:
	void TogglePauseMenu();
};

