#pragma once

#include "SkillComponent.h"

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

	void InitializeProperties(const std::shared_ptr<const CSkillData>& SkillData) override;
};

