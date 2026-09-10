#pragma once
#include "World/Actor.h"

enum class ESlashMoveMode
{
	None = -1,
	Stop,
	Move,
	MAX
};

class CSlash : public CActor
{
public:
	CSlash();
	CSlash(const CSlash& src);
	CSlash(CSlash&& src) noexcept;
	virtual ~CSlash();

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void Destroy();

protected:
	std::weak_ptr<class CMeshComponent> mMeshComponent;
	std::weak_ptr<class CColliderBox2D> mBodyCollision;
	std::weak_ptr<class CColliderBox2D> mHeadCollision;
	std::weak_ptr<class CAnimation2DComponent> mAnimationComponent;
	std::weak_ptr<class CProjectileMovementComponent> mProjectileMoveComponent;

	bool mIsHitTarget = false;
	bool mIsPlayAnimation = false;

	float mAliveTime = 1.f;
	float mElapsedTime = 0.f;

	float mDamage = 1.f;

	ESlashMoveMode mMoveMode = ESlashMoveMode::None;
	FVector3 mDirection = FVector3(-1.f, -1.f, 0.f);
	
	void PlayAnimation();
	void OnBeginOverlap(const FVector3& HitPoint, const FVector3& Normal, std::weak_ptr<class CCollider> Collider);
	void LookAtDirection(const FVector3& Direction);

public:
	void InitializeProperties(const FVector3& Direciton, float Damage, ESlashMoveMode Mode);
};

