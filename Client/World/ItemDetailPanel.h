#pragma once
#include "World/WidgetContainer.h"
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

	// 일단 이름과 아이콘만 출력하게 해보겠숨.
public:
	void SetIconTexture(std::string Name, const TCHAR* Path);
	void SetNameText(const TCHAR* Name);
};

