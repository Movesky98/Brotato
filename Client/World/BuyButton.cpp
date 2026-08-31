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
			Button->SetTexture((EWidgetState::Type)i, "BuyButton", TEXT("Custom/Shop/RoundBox_Grey_Buy.png"));
		}
	}

	auto Icon = mIcon.lock();
	if (Icon)
	{
		Icon->SetPos(77.f, 8.f);
		Icon->SetSize(30.f, 30.f);
		Icon->SetTexture("MaterialIcon", TEXT("Custom/Material/Harvesting_Icon.png"));
		Icon->SetTint(1.f, 1.f, 1.f, 1.f);
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
