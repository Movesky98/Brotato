#pragma once
#include "Weapon.h"

enum class EMeleeAttackPhase
{
	None = -1,	// 공격 전 대기상태
	Approach,	// 적과 떨어진 거리만큼 근접하는 상태
	Swing,		// 휘두르는 상태
	Return,		// 원래 위치로 돌아오는 상태
	MAX
};

class CMeleeWeapon :
    public CWeapon
{
public:
	CMeleeWeapon();
	CMeleeWeapon(const CMeleeWeapon& src);
	CMeleeWeapon(CMeleeWeapon&& src) noexcept;
	virtual ~CMeleeWeapon();

protected:
	std::weak_ptr<class CMovementComponent> mMoveComp;
	std::weak_ptr<class CColliderBox2D> mBoxCollision;

	std::weak_ptr<class CEnemy> mTarget;

	FVector3 mSpawnDirection = FVector3::Zero;	// 공격 시작하기 전, 원래 위치
	float mDeltaDistance = 0.f;
	float mDistance = 0.f;			// 공격할 때 이동할 거리

	float mDeltaAngle = 0.f;
	float mSwingAngle = 0.f;		// 휘두를 때 타겟 중심 각도
	float mSpread = 45.f;			// 중심 각도를 기준으로 ± Spread만큼 휘두를 거임
	float mDefaultSpeed = 300.f;

	EMeleeAttackPhase mAttackPhase = EMeleeAttackPhase::None;

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void Destroy();

protected:
	virtual CMeleeWeapon* Clone()
	{
		return new CMeleeWeapon(*this);
	}

	void InitializeSettings(const std::weak_ptr<CWeaponData>& InData, const FStat& Stat) override;

	void Attack(std::weak_ptr<class CEnemy> TargetPtr) override;
	void HandleAttackPhase(float DeltaTime);
	void OnHitCollisionBeginOverlap(const FVector3& HitPoint, const FVector3& Normal, std::weak_ptr<class CCollider> Collider);

	virtual void SetTexture(const std::string& Name, const std::wstring& TexturePath, float SizeX, float SizeY);
};

