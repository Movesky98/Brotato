#pragma once
#include "World/WidgetContainer.h"

class CImage;
class CButton;

class CTitleScreen :
    public CWidgetContainer
{
public:
	CTitleScreen();
	CTitleScreen(const CTitleScreen& src);
	virtual ~CTitleScreen();

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);

public:
	virtual CTitleScreen* Clone();

protected:
	std::weak_ptr<CImage> mBrotatoImage;
	std::weak_ptr<CImage> mMistBackImage;
	std::weak_ptr<CImage> mMistFrontImage;

	std::weak_ptr<CButton> mContinueButton;
	std::weak_ptr<CButton> mStartButton;
	std::weak_ptr<CButton> mProfileButton;
	std::weak_ptr<CButton> mCodexButton;
	std::weak_ptr<CButton> mOptionButton;
	std::weak_ptr<CButton> mQuitButton;

	void CreateImage();
	std::weak_ptr<CButton> CreateButton(const std::string& Name, const TCHAR* Text, const FVector2& Position, bool IsActive = false);

	void OnClickedStartButton();
	void OnClickedQuitButton();

	float mStartPos = -100.f;
	float mGoalPos = 0.f;

	float mProgress = 0.f;
	float mPeriod = 6.f;
};

