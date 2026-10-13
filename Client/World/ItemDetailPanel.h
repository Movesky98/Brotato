#pragma once
#include "World/WidgetContainer.h"

struct FStatLineDesc;
struct FWeaponStatLineDesc;
enum class EPrimaryStats;

class CItemDetailPanel :
    public CWidgetContainer
{
public:
	CItemDetailPanel();
	CItemDetailPanel(const CItemDetailPanel& src);
	virtual ~CItemDetailPanel();

public:
	virtual bool Init();

public:
	virtual CItemDetailPanel* Clone();

protected:
	std::weak_ptr<class CImage> mBackgroundImage;
	std::weak_ptr<class CImage> mBackgroundFrame;
	std::weak_ptr<class CImage> mIconFrame;
	std::weak_ptr<class CImage> mIcon;
	std::weak_ptr<class CTextBlock> mNameText;

	// 아이템(CStatLine)과 무기(CWeaponStatLine) 줄을 함께 담는다.
	std::vector<std::weak_ptr<class CWidgetContainer>> mStatLines;

	void ClearStatLines();
	void CreateStatLines(const std::vector<FStatLineDesc>& LineDescs);
	void CreateWeaponStatLines(const std::vector<FWeaponStatLineDesc>& LineDescs);

public:
	// 박스 크기마다 텍스처를 따로 그려야 해서(9-slice 없음) 이름, 경로, 크기를 호출하는 쪽에서 정한다.
	void SetBackgroundImage(const std::string& Name, const TCHAR* Path, const FVector2& Size);
	void SetBackgroundFrame(const std::string& Name, const TCHAR* Path, const FVector2& Size);
	void SetFrameTint(const FVector4& Tint);

	void SetShopDataDesc(std::weak_ptr<class CShopData> Data);
	void SetStatDesc(const EPrimaryStats& Stat, const int Value);
};

