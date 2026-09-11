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
	std::weak_ptr<class CMeshComponent> mMeshComp;
	std::weak_ptr<class CMeshComponent> mLegMeshComp;
	std::weak_ptr<class CCameraComponent> mCamera;
	std::weak_ptr<class CColliderBox2D> mBox;
	std::weak_ptr<class CColliderSphere2D> mPickupRange;

	std::weak_ptr<class CCharacterMovementComponent> mMoveComp;
	std::weak_ptr<class CAnimation2DComponent> mAnimComp;
	std::weak_ptr<CInventoryComponent> mInventoryComponent;

	std::vector<std::weak_ptr<class CWeapon>> Weapons;

	//UV 스크롤링용 변수
	float mUVScrollTime = 0.f;

	//Dissolve 효과 변수
	bool mIsDissolve = false;
	float mDissolveProgress = 0.f;
	float mDissolveSpeed = 0.3f;

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

	void OnDeath();
	void OnPickupRangeBeginOverlap(const FVector3& HitPoint, const FVector3& Normal, std::weak_ptr<class CCollider> Collider);

public:
	void MoveForward();
	void MoveBackward();
	void MoveRight();
	void MoveLeft();

	void Dissolve();

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

