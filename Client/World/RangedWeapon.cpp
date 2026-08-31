#include "RangedWeapon.h"

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

	//auto MeshComp = mMeshComp.lock();

	//if (MeshComp)
	//{
	//	MeshComp->SetMesh("TexRect");
	//	MeshComp->SetShader("Material");
	//	MeshComp->AddTexture(0, "RangedWeapon", TEXT("Custom/pistol.png"));
	//	MeshComp->AddTexture(0, "DissolveTexture", TEXT("dissolve_smoke.png"), "Texture", 1);
	//	MeshComp->SetBlendState(0, "AlphaBlend");
	//	MeshComp->SetRelativeScale(40.f, 40.f);
	//}

	//auto SphereCollision = mSphereCollision.lock();
	//if (SphereCollision)
	//{
	//	SphereCollision->SetRadius(300.f);
	//}

	//mAttackDelay = 0.1f;

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
