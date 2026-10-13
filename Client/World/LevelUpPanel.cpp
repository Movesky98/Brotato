#include "LevelUpPanel.h"

#include "GameTypes.h"
#include "StatTable.h"
#include "ItemDetailPanel.h"
#include "InputPromptButton.h"

#include "World/Button.h"
#include "World/TextBlock.h"

CLevelUpPanel::CLevelUpPanel()
{
}

CLevelUpPanel::CLevelUpPanel(const CLevelUpPanel& src)
    : CWidgetContainer(src)
{
}

CLevelUpPanel::~CLevelUpPanel()
{
}

bool CLevelUpPanel::Init()
{
    if (!CWidgetContainer::Init()) return false;

    for (int i = 0; i < 4; ++i)
    {
        mUpgradeCards[i].mCard = CreateWidget<CItemDetailPanel>("UpgradeCard");
        mUpgradeCards[i].mButton = CreateWidget<CInputPromptButton>("SelectButton");
        
        auto UpgradeCard = mUpgradeCards[i].mCard.lock();
        if (UpgradeCard)
        {
            UpgradeCard->SetPos(22.f + 246.f * i, 252.f);
            UpgradeCard->SetSize(240.f, 170.f);

            UpgradeCard->SetBackgroundFrame("UpgradeCard_BackgroundFrame", TEXT("Custom/RewardMenu/RoundBox_UpgradeCardFrame.png"), FVector2(240.f, 217.f));
            UpgradeCard->SetBackgroundImage("UpgradeCard_BackgroundImage", TEXT("Custom/RewardMenu/RoundBox_UpgradeCard.png"), FVector2(240.f, 217.f));
        }

        auto SelectButton = mUpgradeCards[i].mButton.lock();
        if (SelectButton)
        {
            SelectButton->SetPos(35.f + 246.f * i, 422.f);
            SelectButton->SetSize(215.f, 34.f);
            SelectButton->SetButtonTexture("UpgradeSelectButton", TEXT("Custom/RewardMenu/RoundBox_UpgradeSelect.png"));
            SelectButton->SetButtonTint(FVector4(0.2f, 0.2f, 0.2f, 1.f));

            SelectButton->SetLabelText(TEXT("Select"));
            SelectButton->SetLabelTextFontSize(24.f);

            SelectButton->SetKeyIconEnable(false);
            SelectButton->SetTrailingIconEnable(false);

            SelectButton->SetClickedFunction(this, &CLevelUpPanel::OnSelectButtonClicked);
        }
    }

    mRerollButton = CreateWidget<CInputPromptButton>("RerollButton");

    auto RerollButton = mRerollButton.lock();
    if (RerollButton)
    {
        RerollButton->SetPos(348.f, 503.f);
        RerollButton->SetSize(325.f, 43.f);
        RerollButton->SetButtonTexture("UpgradeRerollButton", TEXT("Custom/RewardMenu/RoundBox_UpgradeReroll.png"));
        RerollButton->SetButtonTint(FVector4(0.25f, 0.25f, 0.25f, 1.f));

        RerollButton->SetLabelText(TEXT("초기화"));
        RerollButton->SetLabelTextFontSize(24.f);

        RerollButton->SetKeyIconPos(14.f, 9.f);
        RerollButton->SetKeyIconSize(25.f, 25.f);
        RerollButton->SetKeyIconTexture("Key_F", TEXT("Custom/Shop/Key_F.png"));

        RerollButton->SetTrailingIconPos(230.f, 7.f);
        RerollButton->SetTrailingIconSize(28.f, 28.f);
        RerollButton->SetTrailingIconTexture("MaterialIcon", TEXT("Custom/Material/Harvesting_Icon.png"));
        RerollButton->SetTrailingIconTint(FVector4(0.462745f, 1.f, 0.462745f, 1.f));

        RerollButton->SetClickedFunction(this, &CLevelUpPanel::OnRerollButtonClicked);
    }

    RefreshUpgradeCards();

    return true;
}

CLevelUpPanel* CLevelUpPanel::Clone()
{
    return new CLevelUpPanel(*this);
}

void CLevelUpPanel::OnSelectButtonClicked()
{
    // InputPromptButton은 상태를 Clicked로 바꾼 뒤 클릭 콜백을 부른다. 눌린 버튼만 Clicked 상태다.
    int SelectedIndex = -1;

    for (int i = 0; i < 4; ++i)
    {
        auto SelectButton = mUpgradeCards[i].mButton.lock();
        if (SelectButton && SelectButton->GetWidgetState() == EWidgetState::Clicked)
        {
            SelectedIndex = i;
            break;
        }
    }

    if (SelectedIndex < 0) return;

    if (mCount > 0)
    {
        if (mOnUpgradeCardSelectedFunc)
        {
            mOnUpgradeCardSelectedFunc(mUpgradeCards[SelectedIndex].mStat);
        }
        
        RefreshUpgradeCards();
        mCount--;
    }
    
    if (mCount != 0) return;

    if (mOnLevelUpEndedFunc)
    {
        mOnLevelUpEndedFunc();
    }
}

void CLevelUpPanel::OnRerollButtonClicked()
{
    RefreshUpgradeCards();
}

void CLevelUpPanel::RefreshUpgradeCards()
{
    // 랜덤 스탯 뽑기
    int StatArr[(int)EPrimaryStats::MAX];

    for (int i = 0; i < (int)EPrimaryStats::MAX; ++i)
    {
        StatArr[i] = i;
    }

    for (int i = 0; i < 100; ++i)
    {
        int First = rand() % (int)EPrimaryStats::MAX;
        int Second = rand() % (int)EPrimaryStats::MAX;
        if (First == Second) continue;

        int Temp = StatArr[First];
        StatArr[First] = StatArr[Second];
        StatArr[Second] = Temp;
    }

    // 업그레이드 스탯 카드 생성
    for (int i = 0; i < 4; ++i)
    {
        auto UpgradeCard = mUpgradeCards[i].mCard.lock();
        if (!UpgradeCard) continue;

        mUpgradeCards[i].mStat = (EPrimaryStats)StatArr[i];
        
        UpgradeCard->SetStatDesc(mUpgradeCards[i].mStat, 1);
    }
}
