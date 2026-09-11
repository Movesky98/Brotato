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

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void Destroy();
	virtual float TakeDamage(float fDamage, const std::string& Instigator, const std::weak_ptr<CActor>& CauseActor);

protected:
	virtual CEnemy* Clone()
	{
		return new CEnemy(*this);
	}

protected:
	std::weak_ptr<class CMeshComponent> mMeshComp;
	std::weak_ptr<class CColliderBox2D> mBoxCollision;
	std::weak_ptr<class CColliderSphere2D> mPerceptionRange;
	std::weak_ptr<class CCharacterMovementComponent> mMoveComp;
	std::weak_ptr<class CMeshComponent> mHitMeshComp;
	std::weak_ptr<class CAnimation2DComponent> mAnimationComp;

	std::unordered_multimap<ESkillTrigger, std::weak_ptr<class CSkillComponent>> TriggerSkillMaps;

	void SetSkills(const std::unordered_multimap<ESkillTrigger, std::shared_ptr<CSkillData>>& SkillData);
	std::weak_ptr<class CSkillComponent> CreateSkillComponent(const ESkillComponent Component);

	void UseSkill(const ESkillTrigger Trigger);

	std::function<void(const FVector3& /* DeathPosition */, bool /* IsWaveSpawn */)> mNotifyDeathFunc;

	void InitializeMeshComponent(const std::string Name, const TCHAR* Path, float SizeX, float SizeY);
	void InitializeBoxCollision(float SizeX, float SizeY);
	virtual void InitializePerceptionRange(float Range, bool IsDebugDraw = false);
	void InitializeSpeed(float Speed);

	// 초기 설정 값 이후 DataAsset을 가지고 알아서 값을 설정하는 함수.
	virtual bool PostInitializeProperties(const std::shared_ptr<class CDataAsset>& Data, const std::weak_ptr<CActor> Target);

public:
	virtual bool InitializeProperties(const std::shared_ptr<class CDataAsset>& Data, const std::weak_ptr<CActor> Target, bool IsWaveSpawn = true);

	template<typename T>
	void SetNotifyDeathFunc(T* Object, void (T::* Func)(const FVector3&, bool))
	{
		mNotifyDeathFunc = std::bind(Func, Object, std::placeholders::_1, std::placeholders::_2);
	}

protected:
	/* Hit */
	bool mIsHit = false;
	float mHitTime = 0.f;
	float mHitDuration = 0.3f;

	/* Animation - Move */
	bool mCanScaleSize = true;
	bool mIsGrowing = true;
	
	FVector2 mMaxScale = FVector2::Zero;
	FVector2 mMinScale = FVector2::Zero;
	FVector2 mCurrentScale = FVector2::Zero;
	
	float mScalePeriod = 0.25f;
	float mScaleProgress = 0.f;

	void PlayMoveAnimation(float DeltaTime);

	/* Animation - Death */
	float mDeathDuration = 0.25f;
	float mDeathRotation = 0.f;
	float mDeathScaleRatio = 1.f;

	void PlayDeathAnimation(float DeltaTime);

	/* Attack */
	bool mCanAttack = false;
	bool mIsTargetInRange = false;
	float mAttackElapsedTime = 0.f;
	float mAttackDelay = 1.f;

	bool mIsKnockBack = true;

	/* AI State */
	std::weak_ptr<CActor> mTarget;

	std::vector<EEnemyMovement> MovementsByRange;
	ERangeState CurrentRangeState = ERangeState::OutOfRange;

	bool mCanMove = true;
	bool mIsRandomMove = false;
	FVector3 RandomPos;

	bool mIsWaveSpawn = true;
	float mRange = 300.f;
	float mSpeed = 10.f;
	float mDamage = 10.f;

	FVector2 mLimitPos;

protected:
	virtual void SetHP(float HP) override;

	void OnCollisionBeginOverlap(const FVector3& HitPoint, const FVector3& Normal, std::weak_ptr<class CCollider> Collider);
	void OnCollisionEndOverlap(std::weak_ptr<class CCollider> Collider);
	void OnPerceptionRangeBeginOverlap(const FVector3& HitPoint, const FVector3& Normal, std::weak_ptr<class CCollider> Collider);
	void OnPerceptionRangeEndOverlap(std::weak_ptr<class CCollider> Collider);
	virtual void OnDeath();

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

