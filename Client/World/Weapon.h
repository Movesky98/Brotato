#pragma once
#include "World/Actor.h"
#include "GameTypes.h"

class CWeaponData;

// Brotato 기준 무기는 각자의 범위가 있고, 각자에게 가장 가까운 적을 노리는 듯.
// Collision의 BeginOverlap, EndOverlap을 통해 접근한 적과 아닌 적을 구분해야 함.

class CWeapon :
    public CActor
{
public:
	CWeapon();
	CWeapon(const CWeapon& src);
	CWeapon(CWeapon&& src) noexcept;
	virtual ~CWeapon();

protected:
	std::weak_ptr<class CMeshComponent> mMeshComp;
	std::weak_ptr<class CColliderSphere2D> mSphereCollision;
	std::weak_ptr<CActor> mOwner;

	std::vector<std::weak_ptr<class CEnemy>> mTargetList;

	float	mElapsedAttackTime	= 0.f;
	bool	mCanAttack			= true;
	bool	mAttackEnded		= true;

	float	mBaseDamage		= 0.f;
	float	mCooldown		= 0.f;
	int		mKnockback		= 0;
	float	mCritical		= 0.f;
	float	mRange			= 0.f;
	float	mLifeSteal		= 0.f;
	int		mPiercing		= 0;

protected:
	void OnCollisionBeginOverlap(const FVector3& HitPoint, const FVector3& Normal, std::weak_ptr<class CCollider> Collider);
	void OnCollisionEndOverlap(std::weak_ptr<class CCollider> Collider);

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void Destroy();

protected:
	virtual CWeapon* Clone()
	{
		return new CWeapon(*this);
	}

protected:
	std::weak_ptr<class CEnemy> GetClosestTarget();
	void LookAtTarget(std::weak_ptr<class CEnemy> Target);
	virtual void Attack(std::weak_ptr<class CEnemy> Target);
	
	virtual void SetTexture(const std::string& Name, const std::wstring& TexturePath, float SizeX, float SizeY);

public:
	virtual void InitializeSettings(const std::weak_ptr<CWeaponData>& InData, const FStat& Stat);
	void SetOwner(const std::weak_ptr<CActor>& Owner) { mOwner = Owner; }
};

