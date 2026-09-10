#pragma once
#include "SkillComponent.h"
class CSpawnEnemyComponent : public CSkillComponent
{
public:
	CSpawnEnemyComponent();
	CSpawnEnemyComponent(const CSpawnEnemyComponent& src);
	CSpawnEnemyComponent(CSpawnEnemyComponent&& src) noexcept;
	virtual ~CSpawnEnemyComponent();

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);

	virtual bool SkillProgress();
	virtual void SkillEnd();

public:
	virtual CSpawnEnemyComponent* Clone() const
	{
		return new CSpawnEnemyComponent(*this);
	}

	void InitializeProperties(const std::shared_ptr<const CSkillData>& SkillData) override;
	// 스폰에는 뭐가 필요하냐...
	// Count, Interval, EnemyID 정도?

	void CreateEnemy();

protected:
	int				mTotalCount = 0;
	int				mSpawnCount = 0;
	float			mElapsedTime = 0.f;
	float			mInterval = 0.f;
	int				mEnemyID = 0;
	ESkillTarget	mTargetType = ESkillTarget::None;

};

