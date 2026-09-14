#include "ShopOfferCard.h"

#include "ShopMenu.h"
#include "BuyButton.h"
#include "ShopData.h"
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
        LockButton->SetButtonTint(FVector4(0.25f, 0.25f, 0.25f, 1.f));

        LockButton->SetKeyIconPos(14.f, 4.f);
        LockButton->SetKeyIconSize(22.f, 22.f);
        LockButton->SetKeyIconTexture("Key_E", TEXT("Custom/Shop/Key_E.png"));

        LockButton->SetTrailingIconPos(199.f, 4.f);
        LockButton->SetTrailingIconSize(22.f, 22.f);
        LockButton->SetTrailingIconTexture("LockedIcon", TEXT("Custom/Shop/Locked_Icon.png"));
        LockButton->SetTrailingIconEnable(false);

        LockButton->SetClickedFunction(this, &CShopOfferCard::OnLockButtonClicked);
        LockButton->SetHoveredFunction(this, &CShopOfferCard::OnLockButtonHovered);
        LockButton->SetUnHoveredFunction(this, &CShopOfferCard::OnLockButtonUnHovered);
    }

    auto BanButton = mBanButton.lock();
    if (BanButton)
    {
        BanButton->SetPos(0.f, 364.f);
        BanButton->SetSize(235.f, 29.f);
        BanButton->SetLabelText(TEXT("BAN"));
        BanButton->SetButtonTint(FVector4(0.25f, 0.25f, 0.25f, 1.f));

        BanButton->SetKeyIconPos(14.f, 4.f);
        BanButton->SetKeyIconSize(22.f, 22.f);
        BanButton->SetKeyIconTexture("Key_R", TEXT("Custom/Shop/Key_R.png"));

        BanButton->SetTrailingIconEnable(false);
        BanButton->SetClickedFunction(this, &CShopOfferCard::OnBanButtonClicked);
    }

    return true;
}

CShopOfferCard* CShopOfferCard::Clone()
{
    return new CShopOfferCard(*this);
}

void CShopOfferCard::OnLockButtonClicked()
{
    mIsLocked = !mIsLocked;
    auto ItemDetailPanel = mItemDetailPanel.lock();
    auto LockButton = mLockButton.lock();
    if (ItemDetailPanel && LockButton)
    {
        if (mIsLocked)
        {
            ItemDetailPanel->SetFrameTint(FVector4::White);
            LockButton->SetLabelTextColor(FVector4::Black);
            LockButton->SetButtonTint(FVector4::White);
        }
        else
        {
            ItemDetailPanel->SetFrameTint(FVector4::Black);
            LockButton->SetLabelTextColor(FVector4::White);
            LockButton->SetButtonTint(FVector4(0.25f, 0.25f, 0.25f, 1.f));
        }
    }


    if(mOnLockButtonClicked)
        mOnLockButtonClicked(GetThisPtr<CShopOfferCard>());
}

void CShopOfferCard::OnLockButtonHovered()
{
    if (mIsLocked) return;

    auto LockButton = mLockButton.lock();
    if (LockButton)
    {
        LockButton->SetTrailingIconEnable(true);
    }
}

void CShopOfferCard::OnLockButtonUnHovered()
{
    if (mIsLocked)
    {
        auto LockButton = mLockButton.lock();
        if (LockButton)
        {
            LockButton->SetLabelTextColor(FVector4::Black);
        }

        return;
    }

    auto LockButton = mLockButton.lock();
    if (LockButton)
    {
        LockButton->SetTrailingIconEnable(false);
    }
}

void CShopOfferCard::OnBanButtonClicked()
{
    if(mOnBanButtonClicked)
        mOnBanButtonClicked(GetThisPtr<CShopOfferCard>());
}

void CShopOfferCard::RequestPurchase()
{
    auto Parent = mParent.lock();
    if (auto ShopMenu = std::dynamic_pointer_cast<CShopMenu>(Parent))
    {
        if (ShopMenu->TryPurchase(mData))
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

    auto ItemDetailPanel = mItemDetailPanel.lock();
    if (ItemDetailPanel)
    {
        // 박스는 구매 버튼까지 감싸므로 패널(264)보다 길다.
        ItemDetailPanel->SetBackgroundImage("OfferCard_BackgroundImage", TEXT("Custom/Shop/RoundBox_Card.png"), FVector2(mSize.x, 320.f));
        ItemDetailPanel->SetBackgroundFrame("OfferCard_BackgroundFrame", TEXT("Custom/Shop/RoundBox_CardFrame.png"), FVector2(mSize.x, 320.f));
    }
}

void CShopOfferCard::SetCardDesc(std::weak_ptr<CShopData> Data, bool bHideBanButton)
{
    auto ShopData = Data.lock();
    if (!ShopData) return;

    mData = ShopData;

    auto ItemDetailPanel = mItemDetailPanel.lock();
    if (ItemDetailPanel)
    {
        ItemDetailPanel->SetShopDataDesc(ShopData);
    }

    auto BuyButton = mBuyButton.lock();
    if (BuyButton)
    {
        BuyButton->SetPriceText(std::to_wstring(ShopData->BasePrice).c_str());
    }

    auto BanButton = mBanButton.lock();
    if (BanButton)
    {
        if (bHideBanButton || ShopData->AssetType == EDataAssetType::Weapon)
        {
            BanButton->SetEnable(false);
        }
        else
            BanButton->SetEnable(true);
    }
}

void CShopOfferCard::SetEnableBanButton(bool IsEnable)
{
    auto BanButton = mBanButton.lock();
    if (BanButton)
    {
        BanButton->SetEnable(IsEnable);
    }
}
