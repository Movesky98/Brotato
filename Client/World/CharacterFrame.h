#pragma once
#include "World/WidgetContainer.h"
#include "StatLine.h"

class CCharacterFrame :
    public CWidgetContainer
{
public:
	CCharacterFrame();
	CCharacterFrame(const CCharacterFrame& src);
	virtual ~CCharacterFrame();

public:
	virtual bool Init();

public:
	virtual CCharacterFrame* Clone();

	std::weak_ptr<class CImage> mMainFrame;
	std::weak_ptr<class CImage> mPreviewFrame;
	std::weak_ptr<class CImage> mStartWeaponFrame;
	std::weak_ptr<class CSlotContainer> mStartWeaponSlotContainer;
	std::weak_ptr<class CImage> mGroundImage;
	std::weak_ptr<class CImage> mCharacterFaceImage;
	std::weak_ptr<class CImage> mCharacterLegImage;
	std::weak_ptr<class CTextBlock> mNameText;
	std::vector<std::weak_ptr<class CStatLine>> mStatLines;
	
	void CreateStatLines(std::vector<std::weak_ptr<class CStatLine>>& OutLines, const std::vector<FStatLineDesc> LineDescs);


public:
	void SetMainFrameTint(FVector4 Tint);
	void SetCharacterDesc(std::weak_ptr<class CDataAsset> DataAsset);
};

