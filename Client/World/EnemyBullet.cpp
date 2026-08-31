#include "EnemyBullet.h"

#include "Player.h"

#include "World/World.h"
#include "World/MeshComponent.h"
#include "World/ColliderSphere2D.h"
#include "World/ProjectileMovementComponent.h"

#include "LogManager.h"


CEnemyBullet::CEnemyBullet()
{
}

CEnemyBullet::CEnemyBullet(const CEnemyBullet& src)
    : CActor(src)
{
}

CEnemyBullet::CEnemyBullet(CEnemyBullet&& src) noexcept
    : CActor(std::move(src))
{
}

CEnemyBullet::~CEnemyBullet()
{
}

bool CEnemyBullet::Init()
{
    if (!CActor::Init()) return false;

    mMeshComp = CreateComponent<CMeshComponent>("Root");
    mCollision = CreateComponent<CColliderSphere2D>("Collision");
    mMoveComp = CreateComponent<CProjectileMovementComponent>("ProjectileMovementComponent");

    auto MeshComp = mMeshComp.lock();
    if (MeshComp)
    {
        MeshComp->SetMesh("TexRect");
        MeshComp->SetShader("Material");
        MeshComp->AddTexture(0, "EnemyBullet", TEXT("Custom/Projectile0000.png"));
        MeshComp->SetRelativeScale(40.f, 40.f);
        MeshComp->SetBlendState(0, "AlphaBlend");
        MeshComp->SetBaseColor(0, FVector4::Red);
    }

    auto Collision = mCollision.lock();
    if (Collision)
    {
        Collision->SetInheritScale(false);
        Collision->SetRadius(10.f);
        Collision->SetDebugDraw(false);
        Collision->SetCollisionProfile("EnemyProjectile");
        Collision->SetBeginOverlapFunc(this, &CEnemyBullet::OnBeginOverlap);
    }

    auto MoveComp = mMoveComp.lock();
    if (MoveComp)
    {
        MoveComp->SetUpdateComp(MeshComp);
        MoveComp->SetSpeed(300.f);
        MoveComp->SetCurrentSpeed(300.f);
    }

    auto World = mWorld.lock();

    if (World)
    {
        mTimerHandle = World->SetTimer(2.f, false, this, &CEnemyBullet::OnTimerEnd);
    }

    return true;
}

void CEnemyBullet::Update(float DeltaTime)
{
    CActor::Update(DeltaTime);

}

void CEnemyBullet::Destroy()
{
    auto World = mWorld.lock();

    if (World)
    {
        World->ClearTimer(mTimerHandle);
    }

    CActor::Destroy();
}


float CEnemyBullet::GetSpeed()
{
    auto MoveComp = mMoveComp.lock();
    if (MoveComp)
    {
        return MoveComp->GetMoveSpeed();
    }

    return 0.0f;
}

float CEnemyBullet::GetCurrentSpeed()
{
    auto MoveComp = mMoveComp.lock();
    if (MoveComp)
    {
        return MoveComp->GetCurrentSpeed();
    }

    return 0.0f;
}

void CEnemyBullet::InitializeProjectile(const FVector3& Dir, float Speed)
{
    auto MoveComp = mMoveComp.lock();
    if (MoveComp)
    {
        MoveComp->SetMoveDir(Dir);
        MoveComp->SetSpeed(Speed);
    }
}

void CEnemyBullet::SetMoveDir(const FVector3& Dir)
{
    auto MoveComp = mMoveComp.lock();

    if (MoveComp)
    {
        MoveComp->SetMoveDir(Dir);
    }
}

void CEnemyBullet::SetSpeed(float Speed)
{
    auto MoveComp = mMoveComp.lock();

    if (MoveComp)
    {
        MoveComp->SetSpeed(Speed);
    }
}

void CEnemyBullet::SetCurrentSpeed(float Speed)
{
    auto MoveComp = mMoveComp.lock();

    if (MoveComp)
    {
        MoveComp->SetCurrentSpeed(Speed);
    }
}

void CEnemyBullet::OnTimerEnd()
{
    LOG_DEBUG("Timer End");

    Destroy();
}

void CEnemyBullet::OnBeginOverlap(const FVector3& HitPoint, const FVector3& Normal, std::weak_ptr<class CCollider> Collider)
{
    auto _Collider = Collider.lock();

    if (_Collider)
    {
        auto HitActor = _Collider->GetOwner().lock();
        if (HitActor)
        {
            HitActor->TakeDamage(1.f, "Enemy", GetThisPtr<CActor>());
        }
    }

    Destroy();
}
