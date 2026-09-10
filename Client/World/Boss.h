#pragma once
#include "Enemy.h"
#include "EnemyTypes.h"

class CBoss :
    public CEnemy
{
public:
	CBoss();
	CBoss(const CBoss& src);
	CBoss(CBoss&& src) noexcept;
	virtual ~CBoss();

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void Destroy();
	virtual float TakeDamage(float fDamage, const std::string& Instigator, const std::weak_ptr<CActor>& CauseActor);

protected:
	virtual CBoss* Clone()
	{
		return new CBoss(*this);
	}

protected:
	std::weak_ptr<class CWidgetComponent> mWidgetComponent;
	
	int mPhase = 0;
	int mMaxPhase = 0;
	std::vector<FMovementType> mPhaseMovements;
	std::vector<std::unordered_multimap<ESkillTrigger, std::shared_ptr<class CSkillData>>> mPhaseSkills;

	bool PostInitializeProperties(const std::shared_ptr<class CDataAsset>& Data, const std::weak_ptr<CActor> Target) override;
	void OnDeath() override;

	void ClearSkills();

	void EnterNextPhase();
};

