#pragma once
#include "World/WidgetContainer.h"
class CCharacterSelectScreen :
    public CWidgetContainer
{
public:
	CCharacterSelectScreen();
	CCharacterSelectScreen(const CCharacterSelectScreen& src);
	virtual ~CCharacterSelectScreen();

public:
	virtual bool Init();

public:
	virtual CCharacterSelectScreen* Clone();

protected:
	std::weak_ptr<class CImage> mBackgroundImage;
	std::weak_ptr<class CInputPromptButton> mBackButton;
	std::weak_ptr<class CCharacterFrame> mCharacterFrame;
	std::weak_ptr<class CSlotContainer> mCharacterSlotContainer;
	std::weak_ptr<class CTextBlock> mMaxDifficultyLabel;
	std::weak_ptr<class CTextBlock> mMaxDifficultyValue;
	std::weak_ptr<class CTextBlock> mEndlessWaveLabel;
	std::weak_ptr<class CTextBlock> mEndlessWaveValue;

	std::weak_ptr<class CButton> mEndlessModebutton;
	std::weak_ptr<class CButton> mBanItemButton;
	std::weak_ptr<class CButton> mCoOpButton;

	void CreateRecordsPanel();
	void CreateRunSettingPanel();

	void OnHoveredCharacterSlot(std::weak_ptr<class CDataAsset> DataAsset);
	void OnClickedCharacterSlot(std::weak_ptr<class CDataAsset> DataAsset);

	std::function<void()> mOnBackButtonClicked;
	std::function<void(std::weak_ptr<class CDataAsset> DataAsset)> mOnCharacterSlotClicked;

	void OnBackButtonClicked();

public:
	template<typename T>
	void SetOnBackButtonClickedFunction(T* Obj, void(T::* Func)())
	{
		mOnBackButtonClicked = std::bind(Func, Obj);
	}

	template<typename T>
	void SetOnCharacterSlotClickedFunction(T* Obj, void(T::* Func)(std::weak_ptr<class CDataAsset> DataAsset))
	{
		mOnCharacterSlotClicked = std::bind(Func, Obj, std::placeholders::_1);
	}
};

