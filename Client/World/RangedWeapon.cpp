#include "RangedWeapon.h"
#include "WeaponData.h"

#include "Enemy.h"
#include "Bullet.h"

#include "World/World.h"
#include "World/MeshComponent.h"
#include "World/ColliderSphere2D.h"

CRangedWeapon::CRangedWeapon()
{
}

CRangedWeapon::CRangedWeapon(const CRangedWeapon& src)
    : CWeapon(src)
{
}

CRangedWeapon::CRangedWeapon(CRangedWeapon&& src) noexcept
    : CWeapon(std::move(src))
{
}

CRangedWeapon::~CRangedWeapon()
{
}

bool CRangedWeapon::Init()
{
    if (!CWeapon::Init()) return false;

    return true;
}

void CRangedWeapon::Update(float DeltaTime)
{
    CWeapon::Update(DeltaTime);

}

void CRangedWeapon::Destroy()
{
    CWeapon::Destroy();
}

void CRangedWeapon::InitializeSettings(const std::weak_ptr<CWeaponData>& InData, const FStat& Stat)
{
	auto Data = InData.lock();
	if (!Data) return;

	mBaseDamage = Data->BaseDamage + Stat.Primary[(int)EPrimaryStats::RangedDamage];
	mBaseDamage += mBaseDamage * (float)(Stat.Primary[(int)EPrimaryStats::Damage] / 100.f);
	if (mBaseDamage < 1) mBaseDamage = 1;

	mCooldown = Data->Cooldown;
	mKnockback = Data->Knockback;
	mCritical = Data->Critical;
	mRange = Data->Range;
	mLifeSteal = Data->LifeSteal;
	mPiercing = Data->Piercing;
	mRange = Data->Range + Stat.Primary[(int)EPrimaryStats::Range];

	CWeapon::InitializeSettings(InData, Stat);
}

void CRangedWeapon::Attack(std::weak_ptr<class CEnemy> TargetPtr)
{
	auto Target = TargetPtr.lock();
	auto World = GetWorld().lock();
	if (Target && World)
	{
		auto Bullet = World->CreateActor<CBullet>("Bullet").lock();

		if (Bullet)
		{
			FVector3 FireDirection = Target->GetWorldPos() - GetWorldPos();
			FireDirection.Normalize();
			Bullet->SetDamage(mBaseDamage);
			Bullet->SetWorldPos(GetWorldPos());
			Bullet->InitializeProjectile(FireDirection, 1500.f);
		}

		mAttackEnded = true;
	}
}
