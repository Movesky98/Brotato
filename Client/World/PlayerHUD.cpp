#include "PlayerHUD.h"

#include "Player.h"
#include "Component/InventoryComponent.h"

#include "World/Image.h"
#include "World/TextBlock.h"
#include "World/ProgressBar.h"

CPlayerHUD::CPlayerHUD()
{
}

CPlayerHUD::CPlayerHUD(const CPlayerHUD& src)
    : CPlayerWidgetContainer(src)
{
}

CPlayerHUD::~CPlayerHUD()
{
}

bool CPlayerHUD::Init()
{
    if (!CPlayerWidgetContainer::Init()) return false;

    mWaveText = CreateWidget<CTextBlock>("WaveText");
    mWaveCountText = CreateWidget<CTextBlock>("WaveCountText");
    mLifeBar = CreateWidget<CProgressBar>("HPBar");
    mXPBar = CreateWidget<CProgressBar>("ProgressBar");
    mMaterialText = CreateWidget<CTextBlock>("MaterialText");

    auto WaveText = mWaveText.lock();
    auto WaveCountText = mWaveCountText.lock();
    if (WaveText && WaveCountText)
    {
        WaveText->SetPos(592.f, 18.f);
        WaveText->SetSize(90.f, 25.f);
        WaveText->SetFontSize(24.f);
        WaveText->SetTextColor(FVector4::White);
        WaveText->SetText(TEXT("Wave N"));
        WaveText->SetAlignH(ETextAlignH::Center);
        WaveText->SetAlignV(ETextAlignV::Middle);

        WaveCountText->SetPos(600.f, 60.f);
        WaveCountText->SetSize(80.f, 48.f);
        WaveCountText->SetFontSize(48.f);
        WaveCountText->SetText(TEXT("0"));
        WaveCountText->SetTextColor(FVector4::White);
        WaveCountText->SetAlignH(ETextAlignH::Center);
        WaveCountText->SetAlignV(ETextAlignV::Middle);
    }
    
    auto LifeBar = mLifeBar.lock();
    if (LifeBar)
    {
        LifeBar->SetPos(10.f, 19.f);
        LifeBar->SetSize(216.f, 30.f);
        LifeBar->SetMinValue(0.f);
        LifeBar->SetMaxValue(100.f);
        LifeBar->SetPercent(1.f);

        LifeBar->SetTexture(EProgressBarImageType::Back, "LifeBarBack", TEXT("Custom/PlayerHUD/BarBG.png"));
        LifeBar->SetTexture(EProgressBarImageType::Fill, "LifeBarFill", TEXT("Custom/PlayerHUD/BarFill.png"));

        LifeBar->SetTint(EProgressBarImageType::Fill, FVector4::Red);
        LifeBar->SetTint(EProgressBarImageType::Back, 1.f, 1.f, 1.f, 1.f);

        
        if(auto BarFrame = CreateWidget<CImage>("BarFrame").lock())
        {
            BarFrame->SetPos(10.f, 19.f);
            BarFrame->SetSize(216.f, 30.f);
            BarFrame->SetTexture("BarFrame", TEXT("Custom/PlayerHUD/BarFrame.png"));
        }

        mLifeText = CreateWidget<CTextBlock>("XPText");
        if (auto LifeText = mLifeText.lock())
        {
            LifeText->SetPos(10.f, 19.f);
            LifeText->SetSize(216.f, 30.f);
            LifeText->SetFontSize(20.f);
            LifeText->SetAlignH(ETextAlignH::Center);
            LifeText->SetTextColor(FVector4::White);
            LifeText->SetText(TEXT("10 / 10"));
        }
    }

    auto XPBar = mXPBar.lock();
    if (XPBar)
    {
        XPBar->SetPos(10.f, 56.f);
        XPBar->SetSize(216.f, 30.f);
        XPBar->SetMinValue(0.f);
        XPBar->SetMaxValue(100.f);
        XPBar->SetPercent(0.0f);

        XPBar->SetTexture(EProgressBarImageType::Back, "XPBarBack", TEXT("Custom/PlayerHUD/BarBG.png"));
        XPBar->SetTexture(EProgressBarImageType::Fill, "XPBarFill", TEXT("Custom/PlayerHUD/BarFill.png"));

        XPBar->SetTint(EProgressBarImageType::Fill, FVector4::Green);
        XPBar->SetTint(EProgressBarImageType::Back, 1.f, 1.f, 1.f, 1.f);

        if (auto BarFrame = CreateWidget<CImage>("BarFrame").lock())
        {
            BarFrame->SetPos(10.f, 56.f);
            BarFrame->SetSize(216.f, 30.f);
            BarFrame->SetTexture("BarFrame", TEXT("Custom/PlayerHUD/BarFrame.png"));
        }

        mXPText = CreateWidget<CTextBlock>("XPText");
        if (auto XPText = mXPText.lock())
        {
            XPText->SetPos(10.f, 56.f);
            XPText->SetSize(210.f, 30.f);
            XPText->SetFontSize(20.f);
            XPText->SetAlignH(ETextAlignH::Right);
            XPText->SetTextColor(FVector4::White);
            XPText->SetText(TEXT("LV.1"));
        }
    }

    auto MaterialIcon = CreateWidget<CImage>("MaterialIcon").lock();
    auto MaterialText = mMaterialText.lock();
    if (MaterialIcon && MaterialText)
    {
        MaterialIcon->SetPos(10.f, 93.f);
        MaterialIcon->SetSize(42.f, 42.f);
        MaterialIcon->SetTexture("MaterialIcon", TEXT("Custom/Material/material_ui.png"));
        MaterialIcon->SetTint(0.462745f, 1.f, 0.462745f, 1.f);

        MaterialText->SetPos(60.f, 93.f);
        MaterialText->SetSize(132.f, 42.f);
        MaterialText->SetFontSize(36.f);
        MaterialText->SetText(TEXT("0"));
        MaterialText->SetAlignH(ETextAlignH::Left);
        MaterialText->SetAlignV(ETextAlignV::Middle);
        MaterialText->SetTextColor(FVector4::White);
    }


    return true;
}

CPlayerHUD* CPlayerHUD::Clone()
{
    return new CPlayerHUD(*this);
}

void CPlayerHUD::SubscribeDelegates(std::weak_ptr<class CPlayer> InPlayer)
{
    if (InPlayer.expired()) return;

    auto Player = InPlayer.lock();
    auto InvComp = Player->GetInventoryComponent().lock();
    if (InvComp && Player)
    {
        Player->SetUpdateHPFunction(this, &CPlayerHUD::SetLifeText);
        InvComp->SetOnAddedMaterialFunction(this, &CPlayerHUD::SetMaterialText);
        InvComp->SetOnUpdatedXPFunction(this, &CPlayerHUD::OnUpdatedXP);
        InvComp->SetOnUpdatedLevelFunction(this, &CPlayerHUD::SetXPText);
    }
}

void CPlayerHUD::UnsubscribeDelegates()
{

}

void CPlayerHUD::OnUpdatedXP(int CurrentXP, int RequiredXP)
{
    float Percent = (float)CurrentXP / (float)RequiredXP;

    SetXPBarPercent(Percent);
}

void CPlayerHUD::SetLifeBarPercent(const float Percent)
{
    auto LifeBar = mLifeBar.lock();
    if (LifeBar)
    {
        LifeBar->SetPercent(Percent);
    }
}

void CPlayerHUD::SetXPBarPercent(const float Percent)
{
    auto XPBar = mXPBar.lock();
    if (XPBar)
    {
        XPBar->SetPercent(Percent);
    }
}

void CPlayerHUD::SetWaveText(int Wave)
{
    auto WaveText = mWaveText.lock();
    if (WaveText)
    {
        std::wstring WaveWStr = L"Wave " + std::to_wstring(Wave);

        WaveText->SetText(WaveWStr.c_str());
    }
}

void CPlayerHUD::SetWaveCountText(int Count)
{
    auto WaveCountText = mWaveCountText.lock();
    if (WaveCountText)
    {
        FVector4 Color = Count <= 5 ? FVector4::Red : FVector4::White;
        
        WaveCountText->SetTextColor(Color);
        WaveCountText->SetText(std::to_wstring(Count).c_str());
    }
}

void CPlayerHUD::SetLifeText(float CurrentHP, float MaxHP)
{
    auto LifeText = mLifeText.lock();
    if (LifeText)
    {
        float Percent = MaxHP ? CurrentHP / MaxHP : 0.f;

        std::wstring WaveWStr = std::to_wstring((int)CurrentHP) + L" / " + std::to_wstring((int)MaxHP);

        LifeText->SetText(WaveWStr.c_str());
        SetLifeBarPercent(Percent);
    }
}

void CPlayerHUD::SetXPText(int XPLevel)
{
    auto XPText = mXPText.lock();
    if (XPText)
    {
        std::wstring WaveWStr = L"LV." + std::to_wstring(XPLevel);

        XPText->SetText(WaveWStr.c_str());
    }
}

void CPlayerHUD::SetMaterialText(int Material)
{
    auto MaterialText = mMaterialText.lock();
    if (MaterialText)
    {
        MaterialText->SetText(std::to_wstring(Material).c_str());
    }
}

void CPlayerHUD::InitializeWithInventory(std::weak_ptr<class CInventoryComponent> InvCompWeak)
{
    auto InvComp = InvCompWeak.lock();
    if (InvComp)
    {
        InvComp->SetOnAddedMaterialFunction(this, &CPlayerHUD::SetMaterialText);

       
    }
}
