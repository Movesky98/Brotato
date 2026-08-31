#include "DashSkillComponent.h"
#include "../DashSkillData.h"

#include "../Enemy.h"

#include "World/MovementComponent.h"

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

    return true;
}

void CDashSkillComponent::Update(float DeltaTime)
{
    CSkillComponent::Update(DeltaTime);

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
    if (!CSkillComponent::SkillProgress()) return false;

    // Target 설정하고
    // 목표 지점으로 대시 -> Update에서
    // 이러면 Update에서 이동 부분과 문제될 수도 있음.

    mSkillUse = true;
    auto EnemyOwner = std::dynamic_pointer_cast<CEnemy>(GetOwner().lock());
    if (EnemyOwner)
    {
        auto MoveComp = EnemyOwner->GetComponent<CMovementComponent>().lock();
        auto Target = EnemyOwner->GetTarget().lock();
        if (Target && MoveComp)
        {
            mTarget = Target;
            EnemyOwner->SetCanMove(false);
            MoveComp->SetSpeed(mSpeed);
            mDashDirection = Target->GetWorldPos() - EnemyOwner->GetWorldPos();
        }
    }

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
