#include "StatPanel.h"

#include "StatRow.h"
#include "StatTable.h"
#include "ConvertString.h"

#include "World/Image.h"
#include "World/Button.h"
#include "World/TextBlock.h"

CStatPanel::CStatPanel()
{
}

CStatPanel::CStatPanel(const CStatPanel& src)
    : CWidgetContainer(src)
{

}

CStatPanel::~CStatPanel()
{
}

bool CStatPanel::Init()
{
    if (!CWidgetContainer::Init()) return false;

    mBackgroundImage = CreateWidget<CImage>("BackgroundImage", 0);
    mPrimaryButton = CreateButton("Primary", TEXT("Primary"), FVector2(9.f, 56.f));
    mSecondaryButton = CreateButton("Secondary", TEXT("Secondary"), FVector2(125.f, 56.f));
    mLevelRow = CreateWidget<CStatRow>("LevelRow");
   
    auto StatText = CreateWidget<CTextBlock>("StatTextBlock", 0).lock();
    if (StatText)
    {
        StatText->SetPos(0.f, 12.f);
        StatText->SetSize(260.f, 32.f);
        StatText->SetFontSize(28.f);
        StatText->SetText(TEXT("Stat"));
        StatText->SetAlignH(ETextAlignH::Center);
        StatText->SetAlignV(ETextAlignV::Middle);
        StatText->SetTextColor(FVector4::White);
    }

    auto BackgroundImage = mBackgroundImage.lock();
    if(BackgroundImage)
    {
        BackgroundImage->SetPos(0.f, 0.f);
        BackgroundImage->SetSize(260.f, 567.f);
        BackgroundImage->SetTexture("StatPanelBG", TEXT("Custom/PauseMenu/Background.png"));
        BackgroundImage->SetTint(0.2f, 0.2f, 0.2f, 0.4f);
    }

    if (auto PrimaryButton = mPrimaryButton.lock())
    {
        PrimaryButton->SetWidgetEventFunc(EWidgetEventState::Clicked, this, &CStatPanel::ShowPrimaryStat);
    }

    if (auto SecondaryButton = mSecondaryButton.lock())
    {
        SecondaryButton->SetWidgetEventFunc(EWidgetEventState::Clicked, this, &CStatPanel::ShowSecondaryStat);
    }

    if (auto LevelRow = mLevelRow.lock())
    {
        LevelRow->SetPos(12.f, 95.f);
        LevelRow->SetSize(232.f, 22.5f);
        LevelRow->SetIconTexture("LevelIcon", TEXT("Custom/Stats/CurrentLevel.png"));
        LevelRow->SetNameText(TEXT("Current Level"));
        LevelRow->SetValueText(1, false);
    }

    CreateStatRows(mPrimaryStatRows, true);
    CreateStatRows(mSecondaryStatRows, false);
    
    mIsShowPrimaryStat ? ShowPrimaryStat() : ShowSecondaryStat();
    
    return true;
}

CStatPanel* CStatPanel::Clone()
{
    return new CStatPanel(*this);
}

std::weak_ptr<CButton> CStatPanel::CreateButton(const std::string& Name, const TCHAR* Text, const FVector2& Position)
{
    std::weak_ptr<CButton> TempButton = CreateWidget<CButton>(Name + std::string("Button"), 1);
    std::weak_ptr<CTextBlock> TempTextBlock = CreateWidget<CTextBlock>(Name + std::string("TextBlock"), 1);

    auto Button = TempButton.lock();
    auto TextBlock = TempTextBlock.lock();
    if (!Button || !TextBlock) return std::weak_ptr<CButton>();

    Button->SetPos(Position.x, Position.y);
    Button->SetSize(116.f, 32.f);
    Button->SetChild(TextBlock);

    for (int i = 0; i < EWidgetState::End; ++i)
    {
        Button->SetTexture((EWidgetState::Type)i, "PauseMenuButton", TEXT("Custom/PauseMenu/Button1.png"));
    }

    TextBlock->SetSize(116.f, 32.f);
    TextBlock->SetText(Text);
    TextBlock->SetFontSize(17.f);
    TextBlock->SetTextColor(FVector4::White);
    TextBlock->SetAlignH(ETextAlignH::Center);
    TextBlock->SetAlignV(ETextAlignV::Middle);

    return TempButton;
}

void CStatPanel::CreateStatRows(std::vector<std::weak_ptr<class CStatRow>>& OutRows, bool IsPrimary)
{
    int TextNum = IsPrimary ? (int)EPrimaryStats::MAX : (int)ESecondaryStats::MAX;
    float StartYPos = IsPrimary ? 127.f : 94.f;
    float RowHeight = IsPrimary ? 22.5f : 20.f;

    for (int i = 0; i < TextNum; i++)
    {
        std::shared_ptr<CStatRow> NewStatRow = CreateWidget<CStatRow>("StatRow", 1).lock();
        if (NewStatRow)
        {
            const wchar_t* Name = IsPrimary ? GetStatName((EPrimaryStats)i) : GetStatName((ESecondaryStats)i);
            const wchar_t* Path = IsPrimary ? GetStatIcon((EPrimaryStats)i) : GetStatIcon((ESecondaryStats)i);
          
            NewStatRow->SetPos(12.f, StartYPos + i * RowHeight);
            NewStatRow->SetSize(232.f, 22.5f);

            NewStatRow->SetIconVisible(IsPrimary);
            NewStatRow->SetIconTexture(ToString(Name), Path);
            NewStatRow->SetNameText(Name);
            NewStatRow->SetValueText(0);

            OutRows.push_back(NewStatRow);
        }
    }
}

void CStatPanel::ShowPrimaryStat()
{
    mIsShowPrimaryStat = true;

    for (auto& _StatRow : mPrimaryStatRows)
    {
        auto StatRow = _StatRow.lock();
        if (StatRow)
        {
            StatRow->SetEnable(true);
        }
    }

    for (auto& _StatRow : mSecondaryStatRows)
    {
        auto StatRow = _StatRow.lock();
        if (StatRow)
        {
            StatRow->SetEnable(false);
        }
    }

    auto LevelRow = mLevelRow.lock();
    if (LevelRow)
    {
        LevelRow->SetEnable(true);
    }
}


void CStatPanel::ShowSecondaryStat()
{
    mIsShowPrimaryStat = false;

    for (auto& _StatRow : mSecondaryStatRows)
    {
        auto StatRow = _StatRow.lock();
        if (StatRow)
        {
            StatRow->SetEnable(true);
        }
    }

    for (auto& _StatRow : mPrimaryStatRows)
    {
        auto StatRow = _StatRow.lock();
        if (StatRow)
        {
            StatRow->SetEnable(false);
        }
    }

    auto LevelRow = mLevelRow.lock();
    if (LevelRow)
    {
        LevelRow->SetEnable(false);
    }
}

void CStatPanel::UpdateStats(const FStat Stat)
{
    for (int i = 0; i < (int)EPrimaryStats::MAX; ++i)
    {
        auto StatRow = mPrimaryStatRows[i].lock();
        if (StatRow)
        {
            StatRow->SetValueText(Stat.Primary[i]);
        }
    }

    for (int i = 0; i < (int)ESecondaryStats::MAX; ++i)
    {
        auto StatRow = mSecondaryStatRows[i].lock();
        if (StatRow)
        {
            StatRow->SetValueText(Stat.Secondary[i]);
        }
    }
}

void CStatPanel::UpdateLevel(int Level)
{
    auto LevelRow = mLevelRow.lock();
    if (LevelRow)
    {
        LevelRow->SetValueText(Level, false);
    }
}

