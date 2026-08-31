#pragma once
#include "World/WidgetContainer.h"

struct FShopOfferCardDesc
{
	int				ID = 0;
	bool			IsWeapon = false;
	const TCHAR*	IconPath = L"";
	std::string		Name = "";
	int				BasePrice = 0;
};

class CShopOfferCard : public CWidgetContainer
{
public:
	CShopOfferCard();
	CShopOfferCard(const CShopOfferCard& src);
	virtual ~CShopOfferCard();

public:
	virtual bool Init();
	virtual CShopOfferCard* Clone();

protected:
	std::weak_ptr<class CImage>					mBackgroundImage;
	std::weak_ptr<class CItemDetailPanel>		mItemDetailPanel;
	std::weak_ptr<class CBuyButton>				mBuyButton;
	std::weak_ptr<class CInputPromptButton>		mLockButton;
	std::weak_ptr<class CInputPromptButton>		mBanButton;

	FShopOfferCardDesc mCardDesc;

	void RequestPurchase();

public:
	virtual void SetSize(float x, float y);
	void SetCardDesc(const FShopOfferCardDesc& CardDesc);
};