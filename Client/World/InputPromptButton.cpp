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
            Button->SetTexture((EWidgetState::Type)i, "BuyButton", TEXT("Custom/Shop/RoundBox_Prompt.png"));
        }

        // 텍스처가 흰색이므로 색은 전적으로 틴트가 결정한다.
        ApplyButtonTint(mBaseTint);
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

void CInputPromptButton::SetLabelTextPos(float x, float y)
{
    auto LabelText = mLabelText.lock();
    if (LabelText)
    {
        LabelText->SetPos(x, y);
    }
}

void CInputPromptButton::SetLabelTextSize(float x, float y)
{
    auto LabelText = mLabelText.lock();
    if (LabelText)
    {
        LabelText->SetSize(x, y);
    }
}

void CInputPromptButton::SetLabelTextFontSize(float Size)
{
    auto LabelText = mLabelText.lock();
    if (LabelText)
    {
        LabelText->SetFontSize(Size);
    }
}

void CInputPromptButton::SetLabelTextColor(const FVector4 Tint)
{
    auto LabelText = mLabelText.lock();
    if (LabelText)
    {
        LabelText->SetTextColor(Tint);
    }
}

void CInputPromptButton::ApplyButtonTint(const FVector4& Tint)
{
    auto Button = mButton.lock();
    if (Button)
    {
        // CollisionMouse 오버라이드 이후 자식 버튼은 상태가 바뀌지 않으므로 전 상태에 같은 값을 넣는다.
        for (int i = 0; i < (int)EWidgetState::End; i++)
        {
            Button->SetTint((EWidgetState::Type)i, Tint);
        }
    }
}

void CInputPromptButton::SetButtonTint(const FVector4& Tint)
{
    mBaseTint = Tint;

    ApplyButtonTint(mBaseTint);
}

void CInputPromptButton::SetButtonTint(const FVector4& Tint, EWidgetState::Type Type)
{
    auto Button = mButton.lock();
    if (Button)
    {
        Button->SetTint(Type, Tint);
    }
}

void CInputPromptButton::SetButtonTexture(std::string Name, const TCHAR* Path)
{
    auto Button = mButton.lock();
    if (Button)
    {
        for (int i = 0; i < (int)EWidgetState::End; i++)
        {
            Button->SetTexture((EWidgetState::Type)i, Name, Path);
        }
    }
}

bool CInputPromptButton::CollisionMouse(std::weak_ptr<CWidget>& Result, const FVector2& MousePos)
{
    // 오버라이드를 통해 자식 위젯의 콜리전 검사를 스킵하도록 함.
    // 버튼의 크기가 InputPromptButton과 같기 때문에 위젯컨테이너의 콜리전 이벤트가 발생하기 전에, 버튼의 콜리전이 발생하게 됨.
    return CWidget::CollisionMouse(Result, MousePos);
}

void CInputPromptButton::MouseHovered()
{
    CWidgetContainer::MouseHovered();

    // 기본 색의 알파는 유지한 채 흰색으로 바꾼다.
    ApplyButtonTint(FVector4(1.f, 1.f, 1.f, mBaseTint.w));

    auto LabelText = mLabelText.lock();
    if (LabelText)
    {
        LabelText->SetTextColor(FVector4::Black);
    }

    if (mOnHovered)
        mOnHovered();
}

void CInputPromptButton::MouseUnHovered()
{
    CWidgetContainer::MouseUnHovered();

    ApplyButtonTint(mBaseTint);

    auto LabelText = mLabelText.lock();
    if (LabelText)
    {
        LabelText->SetTextColor(FVector4::White);
    }

    if (mOnUnHovered)
        mOnUnHovered();
}

void CInputPromptButton::MouseClicked()
{
    CWidgetContainer::MouseClicked();

    if (mOnClicked)
        mOnClicked();
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
        KeyIcon->SetTexture(Name + "_Icon", FilePath);
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
