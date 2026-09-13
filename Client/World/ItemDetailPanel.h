#pragma once
#include "World/WidgetContainer.h"

struct FStatLineDesc;
struct FWeaponStatLineDesc;

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
	std::weak_ptr<class CImage> mIcon;
	std::weak_ptr<class CTextBlock> mNameText;

	// 아이템(CStatLine)과 무기(CWeaponStatLine) 줄을 함께 담는다.
	std::vector<std::weak_ptr<class CWidgetContainer>> mStatLines;

	void ClearStatLines();
	void CreateStatLines(const std::vector<FStatLineDesc>& LineDescs);
	void CreateWeaponStatLines(const std::vector<FWeaponStatLineDesc>& LineDescs);

public:
	void SetShopDataDesc(std::weak_ptr<class CShopData> Data);
};

