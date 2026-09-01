#include "InputPromptButton.h"

#include "World/Image.h"
#include "World/Button.h"
#include "World/TextBlock.h"

CInputPromptButton::CInputPromptButton()
{
}

CInputPromptButton::CInputPromptButton(const CInputPromptButton& src)
    : CWidgetContainer(src)
{
}

CInputPromptButton::~CInputPromptButton()
{
}

bool CInputPromptButton::Init()
{
    if (!CWidgetContainer::Init()) return false;

    mButton = CreateWidget<CButton>("Button");
    mKeyIcon = CreateWidget<CImage>("KeyIcon");
    mLabelText = CreateWidget<CTextBlock>("LabelText");
    mTrailingIcon = CreateWidget<CImage>("TrailingIcon");

    auto Button = mButton.lock();
    if (Button)
    {
        Button->SetPos(0.f, 0.f);
        Button->SetSize(mSize);

        for (int i = 0; i < EWidgetState::End; ++i)
        {
            Button->SetTexture((EWidgetState::Type)i, "BuyButton", TEXT("Custom/Shop/RoundBox_Grey_Prompt.png"));
        }
    }

    auto KeyIcon = mKeyIcon.lock();
    if (KeyIcon)
    {
        KeyIcon->SetPos(22.f, 4.f);
        KeyIcon->SetSize(26.f, 22.f);
    }

    auto LabelText = mLabelText.lock();
    if (LabelText)
    {
        LabelText->SetPos(0.f, 0.f);
        LabelText->SetSize(mSize);
        LabelText->SetTextColor(FVector4::White);
        LabelText->SetAlignH(ETextAlignH::Center);
        LabelText->SetAlignV(ETextAlignV::Middle);
    }

    auto TrailingIcon = mTrailingIcon.lock();
    if (TrailingIcon)
    {
        TrailingIcon->SetPos(187.f, 4.f);
        TrailingIcon->SetSize(26.f, 22.f);
    }

    return true;
}

CInputPromptButton* CInputPromptButton::Clone()
{
    return new CInputPromptButton(*this);
}

void CInputPromptButton::SetSize(float x, float y)
{
    CWidgetContainer::SetSize(x, y);

    auto Button = mButton.lock();
    auto LabelText = mLabelText.lock();
    if (Button && LabelText)
    {
        Button->SetSize(x, y);
        LabelText->SetSize(x, y);
    }
}

void CInputPromptButton::SetLabelText(const TCHAR* Text)
{
    auto LabelText = mLabelText.lock();
    if (LabelText)
    {
        LabelText->SetText(Text);
    }
}

void CInputPromptButton::SetKeyIconPos(float x, float y)
{
    auto KeyIcon = mKeyIcon.lock();
    if (KeyIcon)
    {
        KeyIcon->SetPos(x, y);
    }
}

void CInputPromptButton::SetKeyIconSize(float x, float y)
{
    auto KeyIcon = mKeyIcon.lock();
    if (KeyIcon)
    {
        KeyIcon->SetSize(x, y);
    }
}

void CInputPromptButton::SetKeyIconTexture(const std::string Name, const TCHAR* FilePath)
{
    auto KeyIcon = mKeyIcon.lock();
    if (KeyIcon)
    {
        KeyIcon->SetTexture(Name, FilePath);
    }
}

void CInputPromptButton::SetKeyIconEnable(bool IsEnable)
{
    auto KeyIcon = mKeyIcon.lock();
    if (KeyIcon)
    {
        KeyIcon->SetEnable(IsEnable);
    }
}

void CInputPromptButton::SetTrailingIconPos(float x, float y)
{
    auto TrailingIcon = mTrailingIcon.lock();
    if (TrailingIcon)
    {
        TrailingIcon->SetPos(x, y);
    }
}

void CInputPromptButton::SetTrailingIconSize(float x, float y)
{
    auto TrailingIcon = mTrailingIcon.lock();
    if (TrailingIcon)
    {
        TrailingIcon->SetSize(x, y);
    }
}

void CInputPromptButton::SetTrailingIconTexture(const std::string Name, const TCHAR* FilePath)
{
    auto TrailingIcon = mTrailingIcon.lock();
    if (TrailingIcon)
    {
        TrailingIcon->SetTexture(Name, FilePath);
    }
}

void CInputPromptButton::SetTrailingIconTint(FVector4 Tint)
{
    auto TrailingIcon = mTrailingIcon.lock();
    if (TrailingIcon)
    {
        TrailingIcon->SetTint(Tint);
    }
}

void CInputPromptButton::SetTrailingIconEnable(bool IsEnable)
{
    auto TrailingIcon = mTrailingIcon.lock();
    if (TrailingIcon)
    {
        TrailingIcon->SetEnable(IsEnable);
    }
}
