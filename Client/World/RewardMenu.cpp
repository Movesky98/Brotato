#include "RewardMenu.h"

#include "ItemFoundPanel.h"
#include "LevelUpPanel.h"
#include "StatPanel.h"

#include "Player.h"
#include "Component/InventoryComponent.h"

#include "World/Image.h"

CRewardMenu::CRewardMenu()
{
}

CRewardMenu::CRewardMenu(const CRewardMenu& src)
    : CPlayerWidgetContainer(src)
{
}

CRewardMenu::~CRewardMenu()
{
}

bool CRewardMenu::Init()
{
    if (!CPlayerWidgetContainer::Init()) return false;
   
    mBackgroundImage = CreateWidget<CImage>("BackgroundImage", -1);
    mStatPanel = CreateWidget<CStatPanel>("StatPanel");
    mItemFoundPanel = CreateWidget<CItemFoundPanel>("ItemFoundPanel");
    mLevelUpPanel = CreateWidget<CLevelUpPanel>("LevelUpPanel");

    auto BackgroundImage = mBackgroundImage.lock();
    if (BackgroundImage)
    {
        // 텍스처 없이 틴트만 주면 단색으로 그려진다.
        BackgroundImage->SetPos(0.f, 0.f);
        BackgroundImage->SetSize(1280.f, 720.f);
        BackgroundImage->SetTint(0.f, 0.f, 0.f, 0.5f);
    }

    auto StatPanel = mStatPanel.lock();
    if (StatPanel)
    {
        StatPanel->SetPos(1010.f, 132.f);
        StatPanel->SetSize(260.f, 567.f);
    }

    auto ItemFoundPanel = mItemFoundPanel.lock();
    if (ItemFoundPanel)
    {
        ItemFoundPanel->SetPos(0.f, 0.f);
        ItemFoundPanel->SetSize(1280.f, 720.f);
        ItemFoundPanel->SetEnable(false);
    }

    auto LevelUpPanel = mLevelUpPanel.lock();
    if (LevelUpPanel)
    {
        LevelUpPanel->SetPos(0.f, 0.f);
        LevelUpPanel->SetSize(1280.f, 720.f);
        LevelUpPanel->SetEnable(true);
        LevelUpPanel->SetOnLevelUpEndedFunction(this, &CRewardMenu::OnLevelUpEnded);
        LevelUpPanel->SetOnUpgradeCardSelectedFunction(this, &CRewardMenu::OnUpgradeCardSelected);
    }

    return true;
}

CRewardMenu* CRewardMenu::Clone()
{
    return new CRewardMenu(*this);
}

void CRewardMenu::SubscribeDelegates(std::weak_ptr<class CPlayer> InPlayer)
{
    if (InPlayer.expired()) return;

    auto Player = InPlayer.lock();
    auto InvComp = Player->GetInventoryComponent().lock();
    if (InvComp && Player)
    {
        auto StatPanel = mStatPanel.lock();
        if (StatPanel)
        {
            InvComp->SetOnStatUpdatedFunction(StatPanel.get(), &CStatPanel::UpdateStats);
            InvComp->SetOnUpdatedLevelFunction(StatPanel.get(), &CStatPanel::UpdateLevel);
            mInvComp = InvComp;
        }
    }
}

void CRewardMenu::UnsubscribeDelegates()
{

}

void CRewardMenu::OnLevelUpEnded()
{
    if (mOnLevelUpEndedFunc)
    {
        mOnLevelUpEndedFunc();
    }
}

void CRewardMenu::OnUpgradeCardSelected(EPrimaryStats Stat)
{
    auto InvComp = mInvComp.lock();
    if (InvComp)
    {
        InvComp->AddStat(Stat, 1);
    }
}

void CRewardMenu::ShowLevelUpPanel(int LevelCount)
{
    auto LevelUpPanel = mLevelUpPanel.lock();
    if (LevelUpPanel)
    {
        LevelUpPanel->SetEnable(true);
        LevelUpPanel->RefreshUpgradeCards();
        LevelUpPanel->mCount = LevelCount;
    }
}
