#pragma once

#include "SkillComponent.h"
#include "../EnemyTypes.h"

class CDashSkillComponent :
    public CSkillComponent
{
public:
	CDashSkillComponent();
	CDashSkillComponent(const CDashSkillComponent& src);
	CDashSkillComponent(CDashSkillComponent&& src) noexcept;
	virtual ~CDashSkillComponent();

protected:
	std::weak_ptr<class CActor> mTarget;

	FVector3 mDashDirection;
	float mDeltaDistance = 0.f;
	float mDistance = 100.f;
	float mSpeed = 500.f;

	EExpansionMode mExpansionMode = EExpansionMode::None;
	bool mIsInProgressExpansion = false;
	FVector2 mDefaultScale2D = FVector2::Zero;
	float mExpansionScale = 1.2f;		// 최대 크기 배율 (원래 크기 기준)
	float mInflateTime = 0.36f;			// 원래 크기 -> 최대 크기까지 걸리는 시간
	float mDeflateTime = 0.04f;			// 최대 크기 -> 원래 크기까지 걸리는 시간

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void Destroy();

public:
	virtual bool SkillProgress();
	virtual void SkillEnd();

public:
	virtual CDashSkillComponent* Clone() const
	{
		return new CDashSkillComponent(*this);
	}

protected:
	void ScaleOwnerExpansion(const EExpansionMode Mode, float DeltaTime);

public:
	void InitializeProperties(const std::shared_ptr<const CSkillData>& SkillData) override;
};

