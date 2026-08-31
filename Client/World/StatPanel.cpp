#include "StatPanel.h"

#include "StatRow.h"

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

    InitializeStats();
    CreateStatRows(mPrimaryStatRows, true);
    CreateStatRows(mSecondaryStatRows, false);
    
    mIsShowPrimaryStat ? ShowPrimaryStat() : ShowSecondaryStat();
    
    return true;
}

CStatPanel* CStatPanel::Clone()
{
    return new CStatPanel(*this);
}


void CStatPanel::InitializeStats()
{
    mPrimaryStatTexts[(int)EPrimaryStats::MaxHP] = TEXT("MaxHP");
    mPrimaryStatTexts[(int)EPrimaryStats::HPRegeneration] = TEXT("HPRegeneration");
    mPrimaryStatTexts[(int)EPrimaryStats::LifeSteal] = TEXT("LifeSteal");
    mPrimaryStatTexts[(int)EPrimaryStats::Damage] = TEXT("Damage");
    mPrimaryStatTexts[(int)EPrimaryStats::MeleeDamage] = TEXT("MeleeDamage");
    mPrimaryStatTexts[(int)EPrimaryStats::RangedDamage] = TEXT("RangedDamage");
    mPrimaryStatTexts[(int)EPrimaryStats::ElementalDamage] = TEXT("ElementalDamage");
    mPrimaryStatTexts[(int)EPrimaryStats::AttackSpeed] = TEXT("AttackSpeed");
    mPrimaryStatTexts[(int)EPrimaryStats::CritChance] = TEXT("CritChance");
    mPrimaryStatTexts[(int)EPrimaryStats::Engineering] = TEXT("Engineering");
    mPrimaryStatTexts[(int)EPrimaryStats::Range] = TEXT("Range");
    mPrimaryStatTexts[(int)EPrimaryStats::Armor] = TEXT("Armor");
    mPrimaryStatTexts[(int)EPrimaryStats::Dodge] = TEXT("Dodge");
    mPrimaryStatTexts[(int)EPrimaryStats::Speed] = TEXT("Speed");
    mPrimaryStatTexts[(int)EPrimaryStats::Luck] = TEXT("Luck");
    mPrimaryStatTexts[(int)EPrimaryStats::Harvesting] = TEXT("Harvesting");

    mSecondaryStatTexts[(int)ESecondaryStats::ConsumableHeal] = TEXT("ConsumableHeal");
    mSecondaryStatTexts[(int)ESecondaryStats::MaterialsHealing] = TEXT("MaterialsHealing");
    mSecondaryStatTexts[(int)ESecondaryStats::XPGain] = TEXT("XPGain");
    mSecondaryStatTexts[(int)ESecondaryStats::PickupRange] = TEXT("PickupRange");
    mSecondaryStatTexts[(int)ESecondaryStats::ItemsPrice] = TEXT("ItemsPrice");
    mSecondaryStatTexts[(int)ESecondaryStats::ExplosionDamage] = TEXT("ExplosionDamage");
    mSecondaryStatTexts[(int)ESecondaryStats::ExplosionSize] = TEXT("ExplosionSize");
    mSecondaryStatTexts[(int)ESecondaryStats::Bounces] = TEXT("Bounces");
    mSecondaryStatTexts[(int)ESecondaryStats::Piercing] = TEXT("Piercing");
    mSecondaryStatTexts[(int)ESecondaryStats::PiercingDamage] = TEXT("PiercingDamage");
    mSecondaryStatTexts[(int)ESecondaryStats::DamageAgainstBosses] = TEXT("DamageAgainstBosses");
    mSecondaryStatTexts[(int)ESecondaryStats::StructureAttackSpeed] = TEXT("StructureAttackSpeed");
    mSecondaryStatTexts[(int)ESecondaryStats::StructureRange] = TEXT("StructureRange");
    mSecondaryStatTexts[(int)ESecondaryStats::BurningSpeed] = TEXT("BurningSpeed");
    mSecondaryStatTexts[(int)ESecondaryStats::BurningSpread] = TEXT("BurningSpread");
    mSecondaryStatTexts[(int)ESecondaryStats::Knockback] = TEXT("Knockback");
    mSecondaryStatTexts[(int)ESecondaryStats::DoubleMaterialChance] = TEXT("DoubleMaterialChance");
    mSecondaryStatTexts[(int)ESecondaryStats::FreeRerolls] = TEXT("FreeRerolls");
    mSecondaryStatTexts[(int)ESecondaryStats::Trees] = TEXT("Trees");
    mSecondaryStatTexts[(int)ESecondaryStats::Enemies] = TEXT("Enemies");
    mSecondaryStatTexts[(int)ESecondaryStats::EnemySpeed] = TEXT("EnemySpeed");
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
    const TCHAR** Texts = IsPrimary ? mPrimaryStatTexts : mSecondaryStatTexts;
    float StartYPos = IsPrimary ? 127.f : 94.f;
    float RowHeight = IsPrimary ? 22.5f : 20.f;

    for (int i = 0; i < TextNum; i++)
    {
        std::shared_ptr<CStatRow> NewStatRow = CreateWidget<CStatRow>("StatRow", 1).lock();
        if (NewStatRow)
        {
            NewStatRow->SetPos(12.f, StartYPos + i * RowHeight);
            NewStatRow->SetSize(232.f, 22.5f);

            NewStatRow->SetIconVisible(IsPrimary);
            NewStatRow->SetNameText(Texts[i]);
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

