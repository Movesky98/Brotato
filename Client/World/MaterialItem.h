#pragma once
#include "World/Actor.h"
#include "CollectableInterface.h"

class CMaterialItem : public CActor, public ICollectableInterface
{
public:
	CMaterialItem();
	CMaterialItem(const CMaterialItem & src);
	CMaterialItem(CMaterialItem && src) noexcept;
	virtual ~CMaterialItem();

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void Destroy();

protected:
	virtual CMaterialItem* Clone()
	{
		return new CMaterialItem(*this);
	}

protected:
	std::weak_ptr<class CMeshComponent> mMeshComp;
	std::weak_ptr<class CMovementComponent> mMoveComp;
	std::weak_ptr<class CColliderBox2D> mBoxCollision;

	std::weak_ptr<CActor> mTarget;

	std::function<void(int)> mOnAcquiredFunc;

protected:
	void Collect(const std::weak_ptr<CActor>& W_Collector, std::function<void(int)> BindFunc) override;
	void OnCollsiionBeginOverlap(const FVector3& HitPoint, const FVector3& Normal, std::weak_ptr<class CCollider> Collider);

};

