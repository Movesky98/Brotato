#pragma once
#include "World/Actor.h"
#include "Interface/StatusInterface.h"
#include "EnemyTypes.h"

// 여기서 가능한 모든 적을 구현할 수 있도록 해야할 듯.
// 적의 종류별로 필요한 기능들 (스킬, 거리에 따른 이동 방식 등)을 csv에 저장하고
// 그걸 바탕으로 CEnemy class에서 다양하게 만들어지게 하면 편할 것 같은데.

enum class ERangeState
{
	InRange = 0,		// 범위 내
	OutOfRange,			// 범위 밖
	MAX,
};

class CEnemyData;
class CSkillData;

class CEnemy :
	public CActor, public CStatusInterface
{
public:
	CEnemy();
	CEnemy(const CEnemy& src);
	CEnemy(CEnemy&& src) noexcept;
	virtual ~CEnemy();

protected:
	std::weak_ptr<class CMeshComponent> mMeshComp;
	std::weak_ptr<class CColliderBox2D> mBoxCollision;
	std::weak_ptr<class CColliderSphere2D> mPerceptionRange;
	std::weak_ptr<class CCharacterMovementComponent> mMoveComp;
	std::unordered_multimap<ESkillTrigger, std::weak_ptr<class CSkillComponent>> TriggerSkillMaps;

	void SetSkills(const std::unordered_multimap<ESkillTrigger, std::shared_ptr<CSkillData>>& SkillData);
	std::weak_ptr<class CSkillComponent> CreateSkillComponent(const ESkillComponent Component);

	void UseSkill(const ESkillTrigger Trigger);

protected:
	virtual CEnemy* Clone()
	{
		return new CEnemy(*this);
	}

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void Destroy();
	virtual float TakeDamage(float fDamage, const std::string& Instigator, const std::weak_ptr<CActor>& CauseActor);

	bool InitializeEnemyInfo(const std::shared_ptr<const CEnemyData>& Data, const std::weak_ptr<CActor> Target);

protected:
	bool mIsHit = false;
	float mHitTime = 0.f;
	float mHitDuration = 0.3f;

	//Dissolve 효과 변수
	bool mIsDissolve = false;
	float mDissolveProgress = 0.f;
	float mDissolveSpeed = 0.3f;

	bool mCanScaleSize = true;
	float mMaxScaleHeight = 60.f;
	float mMinScaleHeight = 40.f;
	float mCurrentScaleHeight = 40.f;
	float mCurrentScaleWidth = 40.f;
	bool mToggleScale = true;;

	bool mIsTargetInRange = false;

	bool mCanAttack = false;
	float mAttackElapsedTime = 0.f;
	float mAttackDelay = 1.f;

	bool mIsKnockBack = true;

	std::weak_ptr<CActor> mTarget;


	std::vector<EEnemyMovement> MovementsByRange;
	ERangeState CurrentRangeState = ERangeState::OutOfRange;

	bool mCanMove = true;
	bool mIsRandomMove = false;
	FVector3 RandomPos;

	float mHP = 1.f;
	float mSpeed = 10.f;
	float mDamage = 10.f;

	FVector2 mLimitPos;

protected:
	virtual void SetHP(float HP) override;

	void OnCollisionBeginOverlap(const FVector3& HitPoint, const FVector3& Normal, std::weak_ptr<class CCollider> Collider);
	void OnCollisionEndOverlap(std::weak_ptr<class CCollider> Collider);
	void OnPerceptionRangeBeginOverlap(const FVector3& HitPoint, const FVector3& Normal, std::weak_ptr<class CCollider> Collider);
	void OnPerceptionRangeEndOverlap(std::weak_ptr<class CCollider> Collider);
	void OnDeath();

	void PlayMoveAnimation();
	bool PlayHitEffect(float DeltaTime);
	void UpdateBehavior();
	void ChaseTarget();
	void FleeFromTarget();
	void RandomMove();

protected:
	void Attack();

public:
	void NotifyKnockBack(const FVector3& HitPoint, const FVector3& Normal, float Force);

	std::weak_ptr<CActor> GetTarget() const { return mTarget; }

	bool CanMove() const { return mCanMove; }
	void SetCanMove(bool CanMove) { mCanMove = CanMove; }
	bool CanScaleSize() const { return mCanScaleSize; }
	void SetCanScaleSize(bool CanScaleSize) { mCanScaleSize = CanScaleSize; }
	FVector4 GetHitEffectColor() const { return FVector4(0.8f, 0.8f, 0.8f, 1.f); }
};

