#include "Slash.h"

#include "World/MeshComponent.h"
#include "World/ColliderBox2D.h"
#include "World/Animation2DComponent.h"
#include "World/ProjectileMovementComponent.h"

CSlash::CSlash()
{
}

CSlash::CSlash(const CSlash& src)
    : CActor(src)
{
}

CSlash::CSlash(CSlash&& src) noexcept
    : CActor(std::move(src))
{
}

CSlash::~CSlash()
{
}

bool CSlash::Init()
{
    if (!CActor::Init()) return false;

    mMeshComponent = CreateComponent<CMeshComponent>("MeshComponent");
    mBodyCollision = CreateComponent<CColliderBox2D>("BodyCollision");
    mHeadCollision = CreateComponent<CColliderBox2D>("HeadCollision");
    mAnimationComponent = CreateComponent<CAnimation2DComponent>("AnimationComponent");
    mProjectileMoveComponent = CreateComponent<CProjectileMovementComponent>("ProjectileMoveComponent");

    auto MeshComponent = mMeshComponent.lock();
    if (MeshComponent)
    {
        MeshComponent->SetMesh("TexRect");
        MeshComponent->SetShader("Material");
        MeshComponent->SetRelativeScale(240.f, 60.f);
        MeshComponent->AddTexture(0, "Slash", TEXT("Custom/Projectile/Slash_0.png"));
        MeshComponent->SetBlendState(0, "AlphaBlend");
        MeshComponent->SetBaseColor(0, FVector4::White);
    }

    auto BodyCollision = mBodyCollision.lock();
    if (BodyCollision)
    {
        BodyCollision->SetEnable(false);
        BodyCollision->SetInheritScale(false);
        BodyCollision->SetRelativeScale(206.7f, 23.3f);
        BodyCollision->SetRelativePos(-5.3f, 0.f, 0.f);
        BodyCollision->SetBoxSize(206.7f, 23.3f);
        BodyCollision->SetDebugDraw(true);
        BodyCollision->SetCollisionProfile("EnemyProjectile");
        BodyCollision->SetBeginOverlapFunc(this, &CSlash::OnBeginOverlap);
    }

    auto HeadCollision = mHeadCollision.lock();
    if (HeadCollision)
    {
        HeadCollision->SetEnable(false);
        HeadCollision->SetInheritScale(false);
        HeadCollision->SetRelativeScale(12.3f, 12.3f);
        HeadCollision->SetRelativeRotationZ(45.f);
        HeadCollision->SetRelativePos(107.3f, 0.f, 0.f);
        HeadCollision->SetBoxSize(12.3f, 12.3f);
        HeadCollision->SetDebugDraw(true);
        HeadCollision->SetCollisionProfile("EnemyProjectile");
        HeadCollision->SetBeginOverlapFunc(this, &CSlash::OnBeginOverlap);
    }

    auto AnimationComponent = mAnimationComponent.lock();
    if (AnimationComponent)
    {
        AnimationComponent->SetUpdateComponent(mMeshComponent);
    }

    auto ProjectileMoveComponent = mProjectileMoveComponent.lock();
    if (ProjectileMoveComponent)
    {
        ProjectileMoveComponent->SetUpdateComp(mMeshComponent);
        ProjectileMoveComponent->SetEnable(false);
        ProjectileMoveComponent->SetSpeed(200.f);
        ProjectileMoveComponent->SetCurrentSpeed(200.f);
    }

    mAliveTime = 0.75f;

    return true;
}

void CSlash::Update(float DeltaTime)
{
    CActor::Update(DeltaTime);

    if (mIsPlayAnimation) return;

    mElapsedTime += DeltaTime;
    if (mElapsedTime >= mAliveTime)
    {
        PlayAnimation();
    }
}

void CSlash::Destroy()
{

    CActor::Destroy();
}

void CSlash::PlayAnimation()
{
    mIsPlayAnimation = true;

    auto AnimationComponent = mAnimationComponent.lock();
    if (AnimationComponent)
    {
        AnimationComponent->AddAnimation("SlashFrame", 0.5f, 1.f, false);
        AnimationComponent->SetFinishFunction("SlashFrame", this, &CSlash::Destroy);
    }

    auto BodyCollision = mBodyCollision.lock();
    if (BodyCollision)
    {
        BodyCollision->SetEnable(true);
    }
    
    auto HeadCollision = mHeadCollision.lock();
    if (HeadCollision)
    {
        HeadCollision->SetEnable(true);
    }

    auto MoveComponent = mProjectileMoveComponent.lock();
    if (MoveComponent)
    {
        MoveComponent->SetEnable(false);
    }
}

void CSlash::OnBeginOverlap(const FVector3& HitPoint, const FVector3& Normal, std::weak_ptr<class CCollider> Collider)
{
    if (mIsHitTarget) return;

    mIsHitTarget = true;

    // 데미지 처리
    auto ColliderComp = Collider.lock();
    if (!ColliderComp) return;

    auto Owner = ColliderComp->GetOwner().lock();
    if (Owner)
    {
        Owner->TakeDamage(mDamage, "Enemy", GetThisPtr<CActor>());
    }
}

void CSlash::LookAtDirection(const FVector3& Direction)
{
    FVector3 BaseDirection = FVector3(1.f, 0.f, 0.f);

    const float Radian = std::atan2f(Direction.y, Direction.x);
    const float Angle = Radian * 180.f / 3.14159265f;

    SetRelativeRotationZ(Angle);
}

void CSlash::InitializeProperties(const FVector3& Direciton, float Damage, ESlashMoveMode Mode)
{
    mDirection = mDirection;
    mDamage = Damage;

    LookAtDirection(Direciton);

    if (Mode != ESlashMoveMode::Move) return;

    auto MoveComponent = mProjectileMoveComponent.lock();
    if (MoveComponent)
    {
        MoveComponent->SetEnable(true);
        MoveComponent->SetMoveDir(Direciton);
    }
}
