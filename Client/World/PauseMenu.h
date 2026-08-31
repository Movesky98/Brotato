#pragma once
#include "World/WidgetContainer.h"

class CTextBlock;
class CButton;
class CDataAsset;

class CPauseMenu :
    public CWidgetContainer
{
public:
	CPauseMenu();
	CPauseMenu(const CPauseMenu& src);
	virtual ~CPauseMenu();

protected:
	std::weak_ptr<CButton> mResumeButton;		// '재개' 버튼
	std::weak_ptr<CButton> mEndRunButton;		// '달리기를 종료합니다' 버튼
	std::weak_ptr<CButton> mRestartButton;		// '재시작' 버튼
	std::weak_ptr<CButton> mCodexButton;		// '참고서' 버튼
	std::weak_ptr<CButton> mOptionsButton;		// '옵션' 버튼
	std::weak_ptr<CButton> mQuitButton;			// '메인 메뉴로 돌아가기' 버튼

	std::weak_ptr<class CImage> mBackgroundImage;

	std::weak_ptr<class CTextBlock> mWeaponTextBlock;
	std::weak_ptr<class CSlotContainer> mWeaponSlotContainer;

	std::weak_ptr<class CTextBlock> mItemTextBlock;
	std::weak_ptr<class CSlotContainer> mItemSlotContainer;

	std::weak_ptr<class CStatPanel> mStatPanel;

	std::vector<std::shared_ptr<CDataAsset>> mWeaponAssets;
	std::vector<std::shared_ptr<CDataAsset>> mItemAssets;

public:
	virtual bool Init();
	virtual CPauseMenu* Clone();

protected:
	std::weak_ptr<CButton> CreateMenuButton(const std::string& Name, const TCHAR* Text, const FVector2& Position);
	void CreateInventorySlotPanel();

public:
	void UpdateInventory(std::weak_ptr<const class CInventoryComponent> InInventoryComp);
};

