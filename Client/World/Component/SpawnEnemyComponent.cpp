#include "SpawnEnemyComponent.h"

#include "../SpawnEnemySkillData.h"

#include "../MainWorld.h"


CSpawnEnemyComponent::CSpawnEnemyComponent()
{
}

CSpawnEnemyComponent::CSpawnEnemyComponent(const CSpawnEnemyComponent& src)
	: CSkillComponent(src)
{
}

CSpawnEnemyComponent::CSpawnEnemyComponent(CSpawnEnemyComponent&& src) noexcept
	: CSkillComponent(std::move(src))
{
}

CSpawnEnemyComponent::~CSpawnEnemyComponent()
{
}

bool CSpawnEnemyComponent::Init()
{
	if (!CSkillComponent::Init()) return false;

	return true;
}

void CSpawnEnemyComponent::Update(float DeltaTime)
{
	CSkillComponent::Update(DeltaTime);

	if (!mSkillUse) return;

	mElapsedTime += DeltaTime;
	if (mElapsedTime >= mInterval)
	{
		CreateEnemy();

		if (mSpawnCount >= mTotalCount)
			SkillEnd();
	}
}

bool CSpawnEnemyComponent::SkillProgress()
{
	if (!CSkillComponent::SkillProgress()) return false;

	mSkillUse = true;

	return true;
}

void CSpawnEnemyComponent::SkillEnd()
{
	CSkillComponent::SkillEnd();

	mSpawnCount = 0;
	mSkillUse = false;
}

void CSpawnEnemyComponent::InitializeProperties(const std::shared_ptr<const CSkillData>& SkillData)
{
	auto SkillProps = std::dynamic_pointer_cast<const CSpawnEnemySkillData>(SkillData);
	if (SkillProps)
	{
		mSkillCoolTime	= SkillProps->Cooldown;
		mTargetType		= SkillProps->Target;
		mTotalCount		= SkillProps->Count;
		mInterval		= SkillProps->Interval;
		mEnemyID		= SkillProps->EnemyID;
	}
}

void CSpawnEnemyComponent::CreateEnemy()
{
	auto MainWorld = std::dynamic_pointer_cast<CMainWorld>(mWorld.lock());
	if (!MainWorld) return;

	MainWorld->CreateSpawnPoint(ESpawnType::Enemy, mEnemyID);

	mSpawnCount++;
}
