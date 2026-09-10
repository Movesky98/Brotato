#include "Bullet.h"

#include "Enemy.h"

#include "World/MeshComponent.h"
#include "World/ProjectileMovementComponent.h"
#include "World/ColliderBox2D.h"
#include "World/ColliderSphere2D.h"
#include "World/World.h"

#include "LogManager.h"

CBullet::CBullet()
{
}

CBullet::CBullet(const CBullet& src)
	:CActor(src)
{
}

CBullet::CBullet(CBullet&& src) noexcept
	:CActor(std::move(src))
{
}

CBullet::~CBullet()
{
}

float CBullet::GetSpeed()
{
	auto MoveComp = mMoveComp.lock();

	if (MoveComp)
	{
		return MoveComp->GetMoveSpeed();
	}

	return 0.0f;
}

float CBullet::GetCurrentSpeed()
{
	auto MoveComp = mMoveComp.lock();

	if (MoveComp)
	{
		return MoveComp->GetCurrentSpeed();
	}

	return 0.0f;
}

void CBullet::InitializeProjectile(const FVector3& Dir, float Speed)
{
	auto MoveComp = mMoveComp.lock();
	if (MoveComp)
	{
		MoveComp->SetMoveDir(Dir);
		MoveComp->SetSpeed(Speed);
	}

	const FVector3 ZeroAngleVector(1.f, 0.f, 0.f);

	FVector3 CrossProduct = ZeroAngleVector.CrossProduct(Dir);
	float Radian = std::acos(ZeroAngleVector.DotProduct(Dir));
	float Angle = Radian * 180.f / 3.14159265f;

	if (CrossProduct.z <= 0)
		Angle = 360.f - Angle;

	SetRelativeRotationZ(Angle);
}

void CBullet::SetMoveDir(const FVector3& Dir)
{
	auto MoveComp = mMoveComp.lock();

	if (MoveComp)
	{
		MoveComp->SetMoveDir(Dir);
	}
}

void CBullet::SetSpeed(float Speed)
{
	auto MoveComp = mMoveComp.lock();

	if (MoveComp)
	{
		MoveComp->SetSpeed(Speed);
	}
}

void CBullet::SetCurrentSpeed(float Speed)
{
	auto MoveComp = mMoveComp.lock();

	if (MoveComp)
	{
		MoveComp->SetCurrentSpeed(Speed);
	}
}

void CBullet::OnTimerEnd()
{
	LOG_DEBUG("Timer End");

	Destroy();
}

bool CBullet::Init()
{
	CActor::Init();

	mActorTag = "Player";

	mMeshComp = CreateComponent<CMeshComponent>("Root");
	// mCollision = CreateComponent<CColliderSphere2D>("SphereCollision");
	mBox = CreateComponent<CColliderBox2D>("Box");

	mMoveComp = CreateComponent<CProjectileMovementComponent>("MoveComp");

	auto Mesh = mMeshComp.lock();

	if (Mesh)
	{
		Mesh->SetMesh("TexRect");
		Mesh->SetShader("Material");
		Mesh->AddTexture(0, "Bullet", TEXT("Custom/Projectile/Projectile0001.png"));
		Mesh->SetRelativeScale(40.f, 40.f);
		Mesh->SetBlendState(0, "AlphaBlend");
		Mesh->SetBaseColor(0, FVector4(1.f, 1.f, 0.f, 1.f));
	}

	auto Box = mBox.lock();

	if (Box)
	{
		Box->SetInheritScale(true);
		Box->SetBoxSize(25.f, 25.f);
		Box->SetDebugDraw(true);
		Box->SetCollisionProfile("PlayerProjectile");
		Box->SetBeginOverlapFunc(this, &CBullet::OnBeginOverlap);
	}

	auto Collision = mCollision.lock();
	if (Collision)
	{
		Collision->SetInheritScale(false);
		Collision->SetRadius(10.f);
		Collision->SetDebugDraw(true);
		Collision->SetCollisionProfile("PlayerProjectile");
		Collision->SetBeginOverlapFunc(this, &CBullet::OnBeginOverlap);
	}


	auto MoveComp = mMoveComp.lock();

	if (MoveComp)
	{
		MoveComp->SetUpdateComp(Mesh);
		MoveComp->SetSpeed(1000.f);
		MoveComp->SetCurrentSpeed(1000.f);
	}

	auto World = mWorld.lock();

	if (World)
	{
		mTimerHandle = World->SetTimer(2.f, false, this, &CBullet::OnTimerEnd);
	}

	return true;
}

void CBullet::Update(float DeltaTime)
{
	CActor::Update(DeltaTime);

}

void CBullet::Destroy()
{
	auto World = mWorld.lock();

	if (World)
	{
		World->ClearTimer(mTimerHandle);
	}

	CActor::Destroy();
}

void CBullet::OnBeginOverlap(const FVector3& HitPoint, const FVector3& Normal, std::weak_ptr<class CCollider> Collider)
{
	auto _Collider = Collider.lock();

	if (_Collider)
	{
		auto HitActor = _Collider->GetOwner().lock();
		if (HitActor)
		{
			HitActor->TakeDamage(mDamage, "Player", GetThisPtr<CActor>());

			auto Enemy = std::dynamic_pointer_cast<CEnemy>(HitActor);
			auto MoveComp = mMoveComp.lock();
			if (Enemy && MoveComp)
			{
				// 넉백 적용
				Enemy->NotifyKnockBack(HitPoint, Normal, MoveComp->GetMoveSpeed());
			}
		}
	}

	Destroy();
}

