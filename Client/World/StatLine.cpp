#include "StatLine.h"

#include "ConvertString.h"

#include "World/Image.h"
#include "World/TextBlock.h"

CStatLine::CStatLine()
{
}

CStatLine::CStatLine(const CStatLine& src)
    : CWidgetContainer(src)
{
}

CStatLine::~CStatLine()
{
}

bool CStatLine::Init()
{
    if (!CWidgetContainer::Init()) return false;

    mIconImage = CreateWidget<CImage>("IconImage");
    mValueText = CreateWidget<CTextBlock>("ValueTextBlock");
    mNameText = CreateWidget<CTextBlock>("NameTextBlock");

    auto IconImage = mIconImage.lock();
    if (IconImage)
    {
        IconImage->SetPos(0.f, 2.f);
        IconImage->SetSize(18.f, 18.f);
    }

    auto ValueText = mValueText.lock();
    if (ValueText)
    {
        ValueText->SetPos(24.f, 0.f);
        ValueText->SetSize(40.f, 23.f);
        ValueText->SetFontSize(15.f);
        ValueText->SetTextColor(FVector4::White);
        ValueText->SetAlignV(ETextAlignV::Middle);
        ValueText->SetAlignH(ETextAlignH::Left);
    }

    auto NameText = mNameText.lock();
    if (NameText)
    {
        NameText->SetPos(62.f, 0.f);
        NameText->SetSize(210.f, 23.f);
        NameText->SetFontSize(15.f);
        NameText->SetTextColor(FVector4::White);
        NameText->SetAlignV(ETextAlignV::Middle);
        NameText->SetAlignH(ETextAlignH::Left);
    }

    return true;
}

CStatLine* CStatLine::Clone()
{
    return new CStatLine(*this);
}

void CStatLine::Initialize(const FStatLineDesc Desc)
{
    auto NameText = mNameText.lock();
    if (NameText)
    {
        NameText->SetText(Desc.Name);
    }

    auto ValueText = mValueText.lock();
    if (ValueText)
    {
        std::wstring ValueWStr;

        if (Desc.Value > 0)
        {
            ValueWStr += L"+";
            ValueText->SetTextColor(FVector4::Green);
        }
        else
        {
            ValueText->SetTextColor(FVector4::Red);
        }

        ValueWStr += std::to_wstring(Desc.Value);

        ValueText->SetText(ValueWStr.c_str());
    }

    auto Icon = mIconImage.lock();
    if (Icon)
    {
        Icon->SetTexture(ToString(Desc.Name), Desc.IconPath);
    }
}

void CStatLine::SetIconPos(float x, float y)
{
    if (auto Icon = mIconImage.lock())
        Icon->SetPos(x, y);
}

void CStatLine::SetIconSize(float x, float y)
{
    if (auto Icon = mIconImage.lock())
        Icon->SetSize(x, y);
}

void CStatLine::SetValuePos(float x, float y)
{
    if (auto ValueText = mValueText.lock())
        ValueText->SetPos(x, y);
}

void CStatLine::SetValueSize(float x, float y)
{
    if (auto ValueText = mValueText.lock())
        ValueText->SetSize(x, y);
}

void CStatLine::SetValueFontSize(float Size)
{
    if (auto ValueText = mValueText.lock())
        ValueText->SetFontSize(Size);
}

void CStatLine::SetNamePos(float x, float y)
{
    if (auto NameText = mNameText.lock())
        NameText->SetPos(x, y);
}

void CStatLine::SetNameSize(float x, float y)
{
    if (auto NameText = mNameText.lock())
        NameText->SetSize(x, y);
}

void CStatLine::SetNameFontSize(float Size)
{
    if (auto NameText = mNameText.lock())
        NameText->SetFontSize(Size);
}
