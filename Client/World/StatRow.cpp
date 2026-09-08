#include "StatRow.h"

#include "World/Image.h"
#include "World/TextBlock.h"

CStatRow::CStatRow()
{
}

CStatRow::CStatRow(const CStatRow& src)
    : CWidgetContainer(src)
{
}

CStatRow::~CStatRow()
{
}

bool CStatRow::Init()
{
    if (!CWidgetContainer::Init()) return false;

    mIconImage = CreateWidget<CImage>("IconImage", 0);
    mNameText = CreateWidget<CTextBlock>("NameTextBlock", 0);
    mValueText = CreateWidget<CTextBlock>("ValueTextBlock", 0);

    auto IconImage = mIconImage.lock();
    if (IconImage)
    {
        IconImage->SetPos(0.f, 2.f);
        IconImage->SetSize(18.f, 18.f);
    }

    auto NameText = mNameText.lock();
    if (NameText)
    {
        NameText->SetPos(26.f, 0.f);
        NameText->SetSize(150.f, mSize.y);
        NameText->SetFontSize(15.f);
        NameText->SetTextColor(FVector4::White);
        NameText->SetAlignH(ETextAlignH::Left);
    }

    auto ValueText = mValueText.lock();
    if (ValueText)
    {
        ValueText->SetPos(180.f, 0.f);
        ValueText->SetSize(52.f, mSize.y);
        ValueText->SetFontSize(15.f);
        ValueText->SetTextColor(FVector4::White);
        ValueText->SetAlignH(ETextAlignH::Right);
    }

    return true;
}

CStatRow* CStatRow::Clone()
{
    return new CStatRow(*this);
}

void CStatRow::SetColor(const FVector4 Color)
{
    auto NameText = mNameText.lock();
    auto ValueText = mValueText.lock();
    if (NameText && ValueText)
    {
        NameText->SetTextColor(Color);
        ValueText->SetTextColor(Color);
    }
}

void CStatRow::SetSize(float x, float y)
{
    CWidget::SetSize(x, y);

    if (auto NameText = mNameText.lock()) NameText->SetSize(150.f, y);
    if (auto ValueText = mValueText.lock()) ValueText->SetSize(52.f, y);
}

void CStatRow::SetIconVisible(bool IsVisible)
{
    if (auto Icon = mIconImage.lock())
        Icon->SetEnable(IsVisible);

    if (auto Name = mNameText.lock())
        Name->SetPos(IsVisible ? 26.f : 5.f, 0.f);
}

void CStatRow::SetIconTexture(const std::string Name, const TCHAR* Path)
{
    if (auto Icon = mIconImage.lock())
    {
        Icon->SetTexture(Name, Path);
    }
}

void CStatRow::SetImage(const TCHAR* Path)
{
    auto IconImage = mIconImage.lock();
    if (IconImage)
    {
        IconImage->SetTexture("StatRawIcon", Path);
    }
}

void CStatRow::SetNameText(const TCHAR* Text)
{
    auto NameText = mNameText.lock();
    if (NameText)
    {
        NameText->SetText(Text);
    }
}

void CStatRow::SetValueText(int Value)
{
    auto ValueText = mValueText.lock();
    if (ValueText)
    {
        ValueText->SetText(std::to_wstring(Value).c_str());

        if (Value > 0)     SetColor(FVector4::Green);
        if (Value == 0)    SetColor(FVector4::White);
        if (Value < 0)     SetColor(FVector4::Red);
    }
}
