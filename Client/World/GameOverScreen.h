#pragma once
#include "PlayerWidgetContainer.h"
class CGameOverScreen :
    public CPlayerWidgetContainer
{
public:
	CGameOverScreen();
	CGameOverScreen(const CGameOverScreen& src);
	virtual ~CGameOverScreen();

public:
	virtual bool Init();

public:
	virtual CGameOverScreen* Clone();

protected:
	std::weak_ptr<class CImage>		mBackgroundImage;
	std::weak_ptr<class CButton>	mRestartButton;
	std::weak_ptr<class CButton>	mNewRunButton;
	std::weak_ptr<class CButton>	mReturnToTitleScreenButton;
	
	std::weak_ptr<class CImage>					mBackgroundPanel;
	std::weak_ptr<class CStatPanel>				mStatPanel;
	std::weak_ptr<class CSlotContainer>			mWeaponSlotContainer;
	std::weak_ptr<class CSlotContainer>			mItemSlotContainer;
	std::weak_ptr<class CInventoryComponent>	mPlayerInvComp;

	void UpdateResultPanel();

	std::weak_ptr<CButton> CreateButton(const std::string& Name, const TCHAR* Text, const FVector2& Position, const FVector2& Size, bool IsActive = false);
	void CreateResultPanel();
	
	void ToMainWorld();
	void ToStartWorld();

	void SubscribeDelegates(std::weak_ptr<class CPlayer> InPlayer) override;
	void UnsubscribeDelegates() override;

public:
	void InitializeWithInventory(std::weak_ptr<class CInventoryComponent> InvCompWeak);
};

