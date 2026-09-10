#include "Boss.h"

#include "DataAsset.h"
#include "BossData.h"

#include "MainWorld.h"
#include "Player.h"
#include "SkillData.h"
#include "Component/SkillComponent.h"

#include "World/WidgetComponent.h"
#include "World/ProgressBar.h"

#include "World/ColliderSphere2D.h"

CBoss::CBoss()
{
}

CBoss::CBoss(const CBoss& src)
    : CEnemy(src)
{
}

CBoss::CBoss(CBoss&& src) noexcept
    : CEnemy(std::move(src))
{
}

CBoss::~CBoss()
{
}

bool CBoss::Init()
{
    if (!CEnemy::Init()) return false;

    mWidgetComponent = CreateComponent<CWidgetComponent>("WidgetComponent");
    mRange = 600.f;

    auto WidgetComponent = mWidgetComponent.lock();
    if (WidgetComponent)
    {
        WidgetComponent->SetRelativePos(0.f, 75.f, 0.f);

        auto ProgressBar = WidgetComponent->SetWidget<CProgressBar>("BossHPBar").lock();
        if (ProgressBar)
        {
            ProgressBar->SetSize(60.f, 10.f);
            ProgressBar->SetTint(EProgressBarImageType::Back, 0.4f, 0.4f, 0.4f, 1.f);
            ProgressBar->SetTint(EProgressBarImageType::Fill, FVector4::Red);
            ProgressBar->SetTexture(EProgressBarImageType::Back, "BossHPBarBack", TEXT("Custom/Enemy/HPBarBG.png"));
            ProgressBar->SetTexture(EProgressBarImageType::Fill, "BossHPBarFill", TEXT("Custom/Enemy/HPBarFill.png"));
            ProgressBar->SetPercent(1.f);
        }
    }

    return true;
}

void CBoss::Update(float DeltaTime)
{
    CEnemy::Update(DeltaTime);

}

void CBoss::Destroy()
{

    CEnemy::Destroy();
}

float CBoss::TakeDamage(float fDamage, const std::string& Instigator, const std::weak_ptr<CActor>& CauseActor)
{
    float Damage = CEnemy::TakeDamage(fDamage, Instigator, CauseActor);

    auto WidgetComp = mWidgetComponent.lock();
    if (WidgetComp)
    {
        auto Widget = WidgetComp->GetWidget();
        if (Widget)
        {
            auto HPBar = std::dynamic_pointer_cast<CProgressBar>(Widget);
            if (HPBar)
            {
                float Percent = mHP / mHPMax;
                HPBar->SetPercent(Percent);
            }
        }
    }

    if (!mMaxPhase || mHP <= 0.f) return Damage;

    // HP가 다음 페이즈 기준 내에 들어온 경우
    if (mHP < mHPMax - (mHPMax / mMaxPhase) * (mPhase + 1))
    {
        EnterNextPhase();
    }

    return Damage;
}

bool CBoss::PostInitializeProperties(const std::shared_ptr<class CDataAsset>& Data, const std::weak_ptr<CActor> Target)
{
    auto BossData = std::dynamic_pointer_cast<CBossData>(Data);
    if (!BossData) return false;
    if (!BossData->MovementTypes.size()) return false;

    SetHP(BossData->HP);
    mDamage = BossData->Damage;
    mSpeed = BossData->Speed;

    SetDeathFunction(this, &CBoss::OnDeath);
    InitializeSpeed(mSpeed);

    mPhase = 0;
    mMaxPhase = BossData->MovementTypes.size();
    mPhaseMovements = BossData->MovementTypes;
    mPhaseSkills = BossData->Skills;

    // 첫 페이즈 행동 타입
    MovementsByRange.push_back(mPhaseMovements[mPhase].InRange);
    MovementsByRange.push_back(mPhaseMovements[mPhase].OutRange);

    SetSkills(mPhaseSkills[mPhase]);

    mTarget = Target;

    return true;
}

void CBoss::OnDeath()
{
    CEnemy::OnDeath();

}

void CBoss::ClearSkills()
{
    if (!TriggerSkillMaps.size()) return;

    for (const auto& [Trigger, wSkillComp] : TriggerSkillMaps)
    {
        auto SkillComp = wSkillComp.lock();
        if (SkillComp)
        {
            SkillComp->SetAlive(false);
        }
    }

    TriggerSkillMaps.clear();
}

void CBoss::EnterNextPhase()
{
    if (++mPhase > mMaxPhase) return;

    MovementsByRange.clear();

    MovementsByRange.push_back(mPhaseMovements[mPhase].InRange);
    MovementsByRange.push_back(mPhaseMovements[mPhase].OutRange);

    ClearSkills();
    SetSkills(mPhaseSkills[mPhase]);

    auto MainWorld = std::dynamic_pointer_cast<CMainWorld>(mWorld.lock());
    if (!MainWorld) return;

    if (MovementsByRange[(int)CurrentRangeState] != EEnemyMovement::Random)
        mTarget = MainWorld->GetPlayer();
    else
        mTarget.reset();
}
