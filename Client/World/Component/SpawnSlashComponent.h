#pragma once
#include "SkillComponent.h"
#include "../EnemyTypes.h"
#include "../Slash.h"

class CSpawnSlashComponent : public CSkillComponent
{
public:
	CSpawnSlashComponent();
	CSpawnSlashComponent(const CSpawnSlashComponent& src);
	CSpawnSlashComponent(CSpawnSlashComponent&& src) noexcept;
	virtual ~CSpawnSlashComponent();

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);

	virtual bool SkillProgress();
	virtual void SkillEnd();

public:
	virtual CSpawnSlashComponent* Clone() const
	{
		return new CSpawnSlashComponent(*this);
	}

protected:
	int				mSpawnCount			= 0;
	float			mElapsedSpawnTime	= 0.f;
	float			mSpawnInterval		= 0.f;
	int				mSpawnSpread		= 0;
	ESkillTarget	mTargetType			= ESkillTarget::None;
	ESlashMoveMode	SlashMoveMode		= ESlashMoveMode::None;

	void CreateStaticSlash();
	void CreateMovableSlash();
	float GetAimAngleToTarget(float BaseAngle, int Index);

public:
	void InitializeProperties(const std::shared_ptr<const CSkillData>& SkillData) override;
};

