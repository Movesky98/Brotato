#pragma once
#include "PlayerWidgetContainer.h"
#include <unordered_set>
#include <functional>

class CShopMenu : public CPlayerWidgetContainer
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
	void SubscribeDelegates(std::weak_ptr<class CPlayer> InPlayer) override;
	void UnsubscribeDelegates() override;

protected:
	std::weak_ptr<class CImage>					mBackgroundImage;
	std::weak_ptr<class CStatPanel>				mStatPanel;
	std::weak_ptr<class CShopOfferCard>			mOfferCards[4];
	std::weak_ptr<class CSlotContainer>			mItemSlotContainer;
	std::weak_ptr<class CSlotContainer>			mWeaponSlotContainer;
	std::weak_ptr<class CInputPromptButton>		mRerollButton;

	std::weak_ptr<class CButton>				mNextWaveButton;
	std::weak_ptr<class CTextBlock>				mNextWaveText;
	std::weak_ptr<class CTextBlock>				mCurrentWaveText;
	std::weak_ptr<class CInventoryComponent>	mPlayerInvComp;

	std::weak_ptr<class CTextBlock>				mMaterialText;

	std::function<void()> mNextWaveButtonClickedFunc;
	std::unordered_set<int> mBanIDs;

	void OnClickedRerollButton();
	void OnClickedNextWaveButton();
	void OnBanButtonClicked(std::weak_ptr<CShopOfferCard> wOfferCard);
	
	void CreateNextWaveButton(int NextWave);
	void CreateSlotContainer();
	void CreateOfferCards();
	
	// 1. SlotContainer 업데이트
	// 2. StatPanel 업데이트
	void UpdateSlotContainer();
	void SetMaterialText(int Material);

	void AddDataAssetToInventoryComponent();

public:
	template<typename T>
	void SetOnClickedNextWaveFunction(T* Object, void (T::* Func)())
	{
		mNextWaveButtonClickedFunc = std::bind(Func, Object);
	}

	void SetWaveTexts(int Wave);

	virtual void SetUp();
	virtual void TearDown();

	void InitializeWithInventory(std::weak_ptr<class CInventoryComponent> InvCompWeak);
	bool TryPurchase(std::weak_ptr<class CShopData> wShopData);
};

