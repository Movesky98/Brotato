#pragma once
#include "World/WidgetContainer.h"

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
	std::weak_ptr<class CItemDetailPanel>		mItemDetailPanel;
	std::weak_ptr<class CBuyButton>				mBuyButton;
	std::weak_ptr<class CInputPromptButton>		mLockButton;
	std::weak_ptr<class CInputPromptButton>		mBanButton;

	std::weak_ptr<class CShopData>				mData;

	std::function<void(std::weak_ptr<CShopOfferCard> /* this */)> mOnLockButtonClicked;
	std::function<void(std::weak_ptr<CShopOfferCard> /* this */)> mOnBanButtonClicked;

	bool mIsLocked = false;

	void OnLockButtonClicked();
	void OnLockButtonHovered();
	void OnLockButtonUnHovered();

	void OnBanButtonClicked();

	void RequestPurchase();

public:
	virtual void SetSize(float x, float y);
	void SetCardDesc(std::weak_ptr<class CShopData> Data, bool bHideBanButton = false);

	bool IsLocked() const { return mIsLocked; }
	std::weak_ptr<class CShopData> GetShopData() const { return mData; }

	void SetEnableBanButton(bool IsEnable);

	template<typename T>
	void SetLockButtonClickedFunction(T* Obj, void(T::* Func)(std::weak_ptr<CShopOfferCard> /* this */))
	{
		mOnLockButtonClicked = std::bind(Func, Obj, std::placeholders::_1);
	}

	template<typename T>
	void SetBanButtonClickedFunction(T* Obj, void(T::* Func)(std::weak_ptr<CShopOfferCard> /* this */))
	{
		mOnBanButtonClicked = std::bind(Func, Obj, std::placeholders::_1);
	}
};