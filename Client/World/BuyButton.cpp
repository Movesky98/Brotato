#include "BuyButton.h"

#include "World/Image.h"
#include "World/Button.h"
#include "World/TextBlock.h"

CBuyButton::CBuyButton()
{
}

CBuyButton::CBuyButton(const CBuyButton& src)
	: CWidgetContainer(src)
{
}

CBuyButton::~CBuyButton()
{
}

void CBuyButton::SetSize(float x, float y)
{
	mSize.x = x;
	mSize.y = y;

	auto Button = mButton.lock();
	if (Button)
	{
		Button->SetSize(mSize);
	}
}

bool CBuyButton::Init()
{
	if (!CWidgetContainer::Init()) return false;

	mButton = CreateWidget<CButton>("Button");
	mIcon = CreateWidget<CImage>("IconImage");
	mPriceText = CreateWidget<CTextBlock>("PriceTextBlock");

	auto Button = mButton.lock();
	if (Button)
	{
		Button->SetPos(0.f, 0.f);

		for (int i = 0; i < EWidgetState::End; ++i)
		{
			Button->SetTexture((EWidgetState::Type)i, "BuyButton", TEXT("Custom/Shop/RoundBox_Buy.png"));
		}

		// 텍스처를 흰색으로 교체했으므로 기존 회색은 틴트로 맞춘다.
		ApplyButtonTint(mBaseTint);
	}

	auto Icon = mIcon.lock();
	if (Icon)
	{
		Icon->SetPos(77.f, 8.f);
		Icon->SetSize(30.f, 30.f);
		Icon->SetTexture("MaterialIcon", TEXT("Custom/Material/Harvesting_Icon.png"));
		Icon->SetTint(0.462745f, 1.f, 0.462745f, 1.f);
	}

	auto PriceText = mPriceText.lock();
	if (PriceText)
	{
		PriceText->SetPos(27.f, 9.f);
		PriceText->SetSize(48.f, 28.f);
		PriceText->SetText(TEXT("0"));
		PriceText->SetTextColor(FVector4::White);
		PriceText->SetAlignV(ETextAlignV::Middle);
	}

	return true;
}

CBuyButton* CBuyButton::Clone()
{
	return new CBuyButton(*this);
}

void CBuyButton::SetPriceText(const TCHAR* Text)
{
	auto PriceText = mPriceText.lock();
	if (PriceText)
	{
		PriceText->SetText(Text);
	}
}

void CBuyButton::ApplyButtonTint(const FVector4& Tint)
{
	auto Button = mButton.lock();
	if (Button)
	{
		// CollisionMouse 오버라이드 이후 자식 버튼은 상태가 바뀌지 않으므로 전 상태에 같은 값을 넣는다.
		for (int i = 0; i < (int)EWidgetState::End; ++i)
		{
			Button->SetTint((EWidgetState::Type)i, Tint);
		}
	}
}

void CBuyButton::SetButtonTint(const FVector4& Tint)
{
	mBaseTint = Tint;

	ApplyButtonTint(mBaseTint);
}

bool CBuyButton::CollisionMouse(std::weak_ptr<CWidget>& Result, const FVector2& MousePos)
{
	// 오버라이드를 통해 자식 위젯의 콜리전 검사를 스킵하도록 함.
	// 버튼의 크기가 BuyButton과 같기 때문에 위젯컨테이너의 콜리전 이벤트가 발생하기 전에, 버튼의 콜리전이 발생하게 됨.
	return CWidget::CollisionMouse(Result, MousePos);
}

void CBuyButton::MouseHovered()
{
	CWidgetContainer::MouseHovered();

	// 기본 색의 알파는 유지한 채 흰색으로 바꾼다.
	ApplyButtonTint(FVector4(1.f, 1.f, 1.f, mBaseTint.w));

	auto PriceText = mPriceText.lock();
	if (PriceText)
	{
		PriceText->SetTextColor(FVector4::Black);
	}

	if (mOnHovered)
		mOnHovered();
}

void CBuyButton::MouseUnHovered()
{
	CWidgetContainer::MouseUnHovered();

	ApplyButtonTint(mBaseTint);

	auto PriceText = mPriceText.lock();
	if (PriceText)
	{
		PriceText->SetTextColor(FVector4::White);
	}

	if (mOnUnHovered)
		mOnUnHovered();
}

void CBuyButton::MouseClicked()
{
	CWidgetContainer::MouseClicked();

	if (mOnClicked)
		mOnClicked();
}
