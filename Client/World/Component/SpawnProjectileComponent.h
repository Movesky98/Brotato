#pragma once
#include "SkillComponent.h"
#include "../EnemyTypes.h"

class CSpawnProjectileComponent :
    public CSkillComponent
{
public:
	CSpawnProjectileComponent();
	CSpawnProjectileComponent(const CSpawnProjectileComponent& src);
	CSpawnProjectileComponent(CSpawnProjectileComponent&& src) noexcept;
	virtual ~CSpawnProjectileComponent();

protected:
	std::weak_ptr<class CActor> mTarget;

	int mSpawnCount = 1;			// 스킬이 사용될 때 생성 총알 개수
	float mSpawnInterval = 0.f;		// 스킬이 사용될 때 생성 주기
	int mSpawnSpread = 0;			// 스킬이 사용될 때 생성 각도

	int mSpawnedBulletCount = 0;
	float mElapsedSpawnTime = 0.f;

	ESkillTarget mTargetType = ESkillTarget::None;

	EExpansionMode mExpansionMode = EExpansionMode::None;
	bool mIsInProgressExpansion = false;
	FVector2 mDefaultScale2D = FVector2::Zero;
	FVector4 mExpansionEffectColor = FVector4::Red;
	// FVector2 mScaleSpeed2D = FVector2::Zero;

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void Destroy();

public:
	virtual bool SkillProgress();
	virtual void SkillEnd();

public:
	virtual CSpawnProjectileComponent* Clone() const
	{
		return new CSpawnProjectileComponent(*this);
	}

protected:
	void CalculateDirectionBySpread(FVector3& Direction);
	void HandleSkillByTargetType(ESkillTarget Type, float DeltaTime);
	void ScaleOwnerExpansion(const EExpansionMode Mode, float DeltaTime);

public:
	void InitializeProperties(const std::shared_ptr<const CSkillData>& SkillData) override;
};

