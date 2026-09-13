#pragma once

#include "World/Actor.h"
#include "Interface/StatusInterface.h"

class CInventoryComponent;

class CPlayer :
	public CActor, public CStatusInterface
{
public:
	CPlayer();
	CPlayer(const CPlayer& src);
	CPlayer(CPlayer&& src) noexcept;
	virtual ~CPlayer();

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void Destroy();
	virtual float TakeDamage(float fDamage, const std::string& Instigator, const std::weak_ptr<CActor>& CauseActor);

protected:
	virtual CPlayer* Clone()
	{
		return new CPlayer(*this);
	}

protected:
	std::weak_ptr<class CMeshComponent> mBodyMeshComp;
	std::weak_ptr<class CMeshComponent> mLegMeshComp;
	std::weak_ptr<class CCameraComponent> mCamera;
	std::weak_ptr<class CColliderBox2D> mBoxCollision;
	std::weak_ptr<class CColliderSphere2D> mPickupRange;

	std::weak_ptr<class CCharacterMovementComponent> mMoveComp;
	std::weak_ptr<class CAnimation2DComponent> mAnimComp;
	std::weak_ptr<CInventoryComponent> mInventoryComponent;

	std::vector<std::weak_ptr<class CWeapon>> Weapons;

	//Hit
	bool mIsHit = false;
	float mHitTime = 0.f;
	float mHitDuration = 0.3f;

public:
	std::weak_ptr<class CCameraComponent> GetCamera() const
	{
		return mCamera;
	}

	virtual void SetHP(float HP);

protected:
	float mDamage = 1.f;

	/* Animation - Move */
	bool mCanScaleSize = true;
	bool mIsGrowing = true;

	FVector2 mMaxScale = FVector2::Zero;
	FVector2 mMinScale = FVector2::Zero;
	FVector2 mCurrentScale = FVector2::Zero;

	float mScalePeriod = 0.25f;
	float mScaleProgress = 0.f;

	void OnDeath();
	void OnPickupRangeBeginOverlap(const FVector3& HitPoint, const FVector3& Normal, std::weak_ptr<class CCollider> Collider);
	void PlayMoveAnimation(float DeltaTime);

public:
	void MoveForward();
	void MoveBackward();
	void MoveRight();
	void MoveLeft();

	void Hit();

	std::weak_ptr<CInventoryComponent> GetInventoryComponent() { return mInventoryComponent; }

	void SpawnWeapons();
	void DestroyWeapons();
	void SetWeaponSettings();

public:
	void SetPlayerDesc(std::weak_ptr<class CCharacterData> InData);

	void ActiveForWave();
	void DeactiveForWave();
};

