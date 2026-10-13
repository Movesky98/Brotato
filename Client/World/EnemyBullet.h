#pragma once

#include "Projectile.h"
#include "TimeManager.h"

class CEnemyBullet :
    public CProjectile
{
public:
	CEnemyBullet();
	CEnemyBullet(const CEnemyBullet& src);
	CEnemyBullet(CEnemyBullet&& src) noexcept;
	virtual ~CEnemyBullet();

protected:
	std::weak_ptr<class CMeshComponent> mMeshComp;
	std::weak_ptr<class CColliderSphere2D> mCollision;
	std::weak_ptr<class CProjectileMovementComponent> mMoveComp;

	FTimerHandle mTimerHandle;

public:
	float GetSpeed();
	float GetCurrentSpeed();

	void InitializeProjectile(const FVector3& Dir, float Speed);
	void SetMoveDir(const FVector3& Dir);
	void SetSpeed(float Speed);
	void SetCurrentSpeed(float Speed);

	void Test();

	void OnTimerEnd();

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void Destroy();
	void OnBeginOverlap(const FVector3& HitPoint, const FVector3& Normal, std::weak_ptr<class CCollider> Collider);

protected:
	virtual CEnemyBullet* Clone()
	{
		return new CEnemyBullet(*this);
	}

};

