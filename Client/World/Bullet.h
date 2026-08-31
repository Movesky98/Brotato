#pragma once

#include "World/Actor.h"
#include "TimeManager.h"

class CBullet :
	public CActor
{
public:
	CBullet();
	CBullet(const CBullet& src);
	CBullet(CBullet&& src) noexcept;
	virtual ~CBullet();

protected:
	std::weak_ptr<class CMeshComponent> mMeshComp;
	std::weak_ptr<class CColliderSphere2D> mCollision;
	std::weak_ptr<class CColliderBox2D> mBox;

	std::weak_ptr<class CProjectileMovementComponent> mMoveComp;

	FTimerHandle mTimerHandle;

	float mDamage = 0.f;

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void Destroy();
	void OnBeginOverlap(const FVector3& HitPoint, const FVector3& Normal, std::weak_ptr<class CCollider> Collider);

protected:
	virtual CBullet* Clone()
	{
		return new CBullet(*this);
	}

public:
	float GetSpeed();
	float GetCurrentSpeed();

	void InitializeProjectile(const FVector3& Dir, float Speed);
	void SetMoveDir(const FVector3& Dir);
	void SetSpeed(float Speed);
	void SetCurrentSpeed(float Speed);

	void OnTimerEnd();

	void SetDamage(float Damage) { mDamage = Damage; }
};

