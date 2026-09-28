#include "DashSkillComponent.h"
#include "../DashSkillData.h"

#include "../Enemy.h"

#include "World/MovementComponent.h"
#include "World/MeshComponent.h"

CDashSkillComponent::CDashSkillComponent()
{
}

CDashSkillComponent::CDashSkillComponent(const CDashSkillComponent& src)
    : CSkillComponent(src)
{
   
}

CDashSkillComponent::CDashSkillComponent(CDashSkillComponent&& src) noexcept
    : CSkillComponent(std::move(src))
{
}

CDashSkillComponent::~CDashSkillComponent()
{
}

bool CDashSkillComponent::Init()
{
    if (!CSkillComponent::Init()) return false;

    mSkillCoolTime = 1.f;
    mSkillEnable = true;

    auto Owner = GetOwner().lock();
    if (Owner)
    {
        auto MeshComp = Owner->GetComponent<CMeshComponent>().lock();
        if (MeshComp)
        {
            mDefaultScale2D = FVector2(MeshComp->GetRelativeScale().x, MeshComp->GetRelativeScale().y);
        }
    }

    return true;
}

void CDashSkillComponent::Update(float DeltaTime)
{
    CSkillComponent::Update(DeltaTime);

    if (!mSkillStart) return;

    if (mIsInProgressExpansion)
        ScaleOwnerExpansion(mExpansionMode, DeltaTime);

    if (mSkillUse)
    {
        mDeltaDistance += DeltaTime * mSpeed;

        auto Target = mTarget.lock();
        auto EnemyOwner = std::dynamic_pointer_cast<CEnemy>(GetOwner().lock());
        if (Target && EnemyOwner)
        {
            auto MoveComp = EnemyOwner->GetComponent<CMovementComponent>().lock();
            if (!MoveComp) return;

            if (mDistance >= mDeltaDistance)
            {
                MoveComp->SetMoveDir(mDashDirection);
            }
            else
            {
                // 대시는 끝났지만 줄어들기가 끝날 때까지 스킬은 유지
                MoveComp->SetSpeed(MoveComp->GetCurrentSpeed());
                EnemyOwner->SetCanMove(true);

                if (!mIsInProgressExpansion)
                    SkillEnd();
            }
        }
    }
}

void CDashSkillComponent::PostUpdate(float DeltaTime)
{
    CSkillComponent::PostUpdate(DeltaTime);
}

void CDashSkillComponent::Destroy()
{
    CSkillComponent::Destroy();
}

bool CDashSkillComponent::SkillProgress()
{
    if (!CSkillComponent::SkillProgress() || mSkillStart) return false;

    // 부풀리기 -> 대시 + 줄어들기
    // 대시 방향은 스킬 시작 시점에 정한다.

    auto EnemyOwner = std::dynamic_pointer_cast<CEnemy>(GetOwner().lock());
    if (!EnemyOwner) return false;

    auto MoveComp = EnemyOwner->GetComponent<CMovementComponent>().lock();
    auto Target = EnemyOwner->GetTarget().lock();
    if (!Target || !MoveComp) return false;

    mSkillStart = true;
    mTarget = Target;
    mDashDirection = Target->GetWorldPos() - EnemyOwner->GetWorldPos();

    // 부풀리는 동안 이동 정지 (MoveDir은 초기화하지 않으면 이전 방향으로 계속 이동함)
    EnemyOwner->SetCanMove(false);
    EnemyOwner->SetCanScaleSize(false);
    MoveComp->SetMoveDir(FVector3::Zero);
    MoveComp->SetSpeed(mSpeed);

    mIsInProgressExpansion = true;
    mExpansionMode = EExpansionMode::Inflate;

    return true;
}

void CDashSkillComponent::SkillEnd()
{
    CSkillComponent::SkillEnd();

    mSkillUse = false;

    auto Owner = mOwner.lock();
    if (auto EnemyOwner = std::dynamic_pointer_cast<CEnemy>(Owner))
    {
        auto MoveComp = EnemyOwner->GetComponent<CMovementComponent>().lock();
        
        if (MoveComp)
        {
            MoveComp->SetSpeed(MoveComp->GetCurrentSpeed());
        }

        EnemyOwner->SetCanMove(true);
    }

    mDeltaDistance = 0.f;
    mTarget.reset();
    mIsInProgressExpansion = false;
}

void CDashSkillComponent::ScaleOwnerExpansion(const EExpansionMode Mode, float DeltaTime)
{
    auto EnemyOwner = std::dynamic_pointer_cast<CEnemy>(mOwner.lock());
    if (!EnemyOwner) return;

    auto MeshComp = EnemyOwner->GetComponent<CMeshComponent>().lock();
    if (!MeshComp) return;

    FVector2 CurrentScale2D(MeshComp->GetRelativeScale().x, MeshComp->GetRelativeScale().y);

    switch (Mode)
    {
    case EExpansionMode::None:
        CurrentScale2D = mDefaultScale2D;
        mIsInProgressExpansion = false;
        EnemyOwner->SetCanScaleSize(true);

        break;
    case EExpansionMode::Inflate:
    {
        CurrentScale2D += mDefaultScale2D * (mExpansionScale - 1.f) * (DeltaTime / mInflateTime);

        if (CurrentScale2D.x >= mDefaultScale2D.x * mExpansionScale || CurrentScale2D.y >= mDefaultScale2D.y * mExpansionScale)
        {
            CurrentScale2D = mDefaultScale2D * mExpansionScale;
            mSkillUse = true;
            mExpansionMode = EExpansionMode::Deflate;
        }
    }
        break;
    case EExpansionMode::Deflate:
        CurrentScale2D -= mDefaultScale2D * (mExpansionScale - 1.f) * (DeltaTime / mDeflateTime);

        if (CurrentScale2D.x <= mDefaultScale2D.x || CurrentScale2D.y <= mDefaultScale2D.y)
        {
            mExpansionMode = EExpansionMode::None;
        }
        break;
    default:
        break;
    }

    MeshComp->SetRelativeScale(CurrentScale2D);
}

void CDashSkillComponent::InitializeProperties(const std::shared_ptr<const CSkillData>& SkillData)
{
    if (!SkillData) return;

    auto Dash_SkillProps = std::dynamic_pointer_cast<const CDashSkillData>(SkillData);
    if (Dash_SkillProps)
    {
        mSkillCoolTime = Dash_SkillProps->Cooldown;

        mDistance = Dash_SkillProps->Distance;
        mSpeed = Dash_SkillProps->Speed;
    }
}
