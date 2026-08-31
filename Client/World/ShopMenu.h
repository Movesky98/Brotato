#pragma once
#include "World/WidgetContainer.h"
#include <functional>

class CShopMenu : public CWidgetContainer
{
public:
	CShopMenu();
	CShopMenu(const CShopMenu& src);
	virtual ~CShopMenu();

public:
	virtual bool Init();

public:
	virtual CShopMenu* Clone();

protected:
	std::weak_ptr<class CStatPanel>				mStatPanel;
	std::weak_ptr<class CShopOfferCard>			mOfferCards[4];
	std::weak_ptr<class CSlotContainer>			mItemSlotContainer;
	std::weak_ptr<class CSlotContainer>			mWeaponSlotContainer;
	std::weak_ptr<class CInputPromptButton>		mRerollButton;

	std::weak_ptr<class CButton>				mNextWaveButton;
	std::weak_ptr<class CTextBlock>				mNextWaveText;
	std::weak_ptr<class CTextBlock>				mCurrentWaveText;
	std::weak_ptr<class CInventoryComponent>	mPlayerInvComp;

	std::function<void()> mNextWaveButtonClickedFunc;

	void OnClickedRerollButton();
	void OnClickedNextWaveButton();
	
	void CreateNextWaveButton(int NextWave);
	void CreateSlotContainer();
	void CreateOfferCards();
	
	// 1. SlotContainer 업데이트
	// 2. StatPanel 업데이트
	void UpdateSlotContainer();

	void AddDataAssetToInventoryComponent();

public:
	template<typename T>
	void SetOnClickedNextWaveFunction(T* Object, void (T::* Func)())
	{
		mNextWaveButtonClickedFunc = std::bind(Func, Object);
	}

	void SetWaveTexts(int Wave);

	void Show(int Wave);
	void Hide();

	void InitializeWithInventory(std::weak_ptr<class CInventoryComponent> InvCompWeak);
	bool TryPurchase(const int ID, const bool IsWeapon, int Price);
};

