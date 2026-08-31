#include "ShopOfferCard.h"

#include "ShopMenu.h"
#include "BuyButton.h"
#include "ConvertString.h"
#include "ItemDetailPanel.h"
#include "InputPromptButton.h"

#include "World/Image.h"

CShopOfferCard::CShopOfferCard()
{
}

CShopOfferCard::CShopOfferCard(const CShopOfferCard& src)
    : CWidgetContainer(src)
{
}

CShopOfferCard::~CShopOfferCard()
{
}

bool CShopOfferCard::Init()
{
    if (!CWidgetContainer::Init()) return false;

    mBackgroundImage = CreateWidget<CImage>("BackgroundImage");
    mItemDetailPanel = CreateWidget<CItemDetailPanel>("ItemDetailPanel");
    mBuyButton = CreateWidget<CBuyButton>("BuyButton");
    mLockButton = CreateWidget<CInputPromptButton>("LockButton");
    mBanButton = CreateWidget<CInputPromptButton>("BanButton");

    auto ItemDetailPanel = mItemDetailPanel.lock();
    if(ItemDetailPanel)
    {
        ItemDetailPanel->SetPos(0.f, 0.f);
        ItemDetailPanel->SetSize(235.f, 264.f);
    }

    auto BackgroundImage = mBackgroundImage.lock();
    if (BackgroundImage)
    {
        BackgroundImage->SetPos(0.f, 0.f);
        BackgroundImage->SetTexture("RoundBox_Background", TEXT("Custom/Shop/RoundBox_Card.png"));
        BackgroundImage->SetTint(0.f, 0.f, 0.f, 0.5f);
    }

    auto BuyButton = mBuyButton.lock();
    if (BuyButton)
    {
        BuyButton->SetPos(51.f, 267.f);
        BuyButton->SetSize(133.f, 46.f);
        BuyButton->SetClickedButtonFunction(EWidgetEventState::Clicked, this, &CShopOfferCard::RequestPurchase);
    }

    auto LockButton = mLockButton.lock();
    if (LockButton)
    {
        LockButton->SetPos(0.f, 326.f);
        LockButton->SetSize(235.f, 29.f);
        LockButton->SetLabelText(TEXT("Lock"));

        LockButton->SetKeyIconPos(14.f, 4.f);
        LockButton->SetKeyIconSize(22.f, 22.f);
        LockButton->SetKeyIconTexture("Key_E", TEXT("Custom/Shop/Key_E.png"));

        LockButton->SetTrailingIconPos(199.f, 4.f);
        LockButton->SetTrailingIconSize(22.f, 22.f);
        LockButton->SetTrailingIconTexture("LockedIcon", TEXT("Custom/Shop/Locked_Icon.png"));
    }

    auto BanButton = mBanButton.lock();
    if (BanButton)
    {
        BanButton->SetPos(0.f, 364.f);
        BanButton->SetSize(235.f, 29.f);
        BanButton->SetLabelText(TEXT("BAN"));

        BanButton->SetKeyIconPos(14.f, 4.f);
        BanButton->SetKeyIconSize(22.f, 22.f);
        BanButton->SetKeyIconTexture("Key_R", TEXT("Custom/Shop/Key_R.png"));

        BanButton->SetTrailingIconEnable(false);
    }

    return true;
}

CShopOfferCard* CShopOfferCard::Clone()
{
    return new CShopOfferCard(*this);
}

void CShopOfferCard::RequestPurchase()
{
    auto Parent = mParent.lock();
    if (auto ShopMenu = std::dynamic_pointer_cast<CShopMenu>(Parent))
    {
        if (ShopMenu->TryPurchase(mCardDesc.ID, mCardDesc.IsWeapon, mCardDesc.BasePrice))
        {
            // Hide This.
            SetEnable(false);
        }
    }
}

void CShopOfferCard::SetSize(float x, float y)
{
    mSize.x = x;
    mSize.y = y;

    auto BackgroundImage = mBackgroundImage.lock();
    if (BackgroundImage)
    {
        BackgroundImage->SetSize(mSize.x, 320.f);
    }
}

void CShopOfferCard::SetCardDesc(const FShopOfferCardDesc& CardDesc)
{
    mCardDesc = CardDesc;

    auto ItemDetailPanel = mItemDetailPanel.lock();
    if (ItemDetailPanel)
    {
        ItemDetailPanel->SetIconTexture(mCardDesc.Name + std::string("_Icon"), mCardDesc.IconPath);
        ItemDetailPanel->SetNameText(ToWString(mCardDesc.Name).c_str());
    }

    auto BuyButton = mBuyButton.lock();
    if (BuyButton)
    {
        BuyButton->SetPriceText(std::to_wstring(mCardDesc.BasePrice).c_str());
    }
}
