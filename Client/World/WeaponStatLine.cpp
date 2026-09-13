#include "WeaponStatLine.h"

#include "World/TextBlock.h"

CWeaponStatLine::CWeaponStatLine()
{
}

CWeaponStatLine::CWeaponStatLine(const CWeaponStatLine& src)
    : CWidgetContainer(src)
{
}

CWeaponStatLine::~CWeaponStatLine()
{
}

bool CWeaponStatLine::Init()
{
    if (!CWidgetContainer::Init()) return false;

    mNameText = CreateWidget<CTextBlock>("NameTextBlock");
    mValueText = CreateWidget<CTextBlock>("ValueTextBlock");

    // 원본 상점 카드 기준 (라벨은 옅은 베이지, 값은 흰색)
    auto NameText = mNameText.lock();
    if (NameText)
    {
        NameText->SetPos(0.f, 0.f);
        NameText->SetSize(100.f, 18.f);
        NameText->SetFontSize(14.f);
        NameText->SetTextColor(FVector4(234.f / 255.f, 226.f / 255.f, 176.f / 255.f, 1.f));
        NameText->SetAlignV(ETextAlignV::Middle);
        NameText->SetAlignH(ETextAlignH::Left);
    }

    auto ValueText = mValueText.lock();
    if (ValueText)
    {
        ValueText->SetPos(100.f, 0.f);
        ValueText->SetSize(111.f, 18.f);
        ValueText->SetFontSize(14.f);
        ValueText->SetTextColor(FVector4::White);
        ValueText->SetAlignV(ETextAlignV::Middle);
        ValueText->SetAlignH(ETextAlignH::Left);
    }

    return true;
}

CWeaponStatLine* CWeaponStatLine::Clone()
{
    return new CWeaponStatLine(*this);
}

void CWeaponStatLine::Initialize(const FWeaponStatLineDesc Desc)
{
    auto NameText = mNameText.lock();
    if (NameText)
    {
        NameText->SetText(Desc.Name);
    }

    auto ValueText = mValueText.lock();
    if (ValueText)
    {
        ValueText->SetText(Desc.Value.c_str());
    }
}
