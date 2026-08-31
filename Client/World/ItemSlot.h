#pragma once
#include "World/WidgetContainer.h"

class CItemSlot : public CWidgetContainer
{
public:
	CItemSlot();
	CItemSlot(const CItemSlot& src);
	virtual ~CItemSlot();

public:
	virtual bool Init();
	virtual void MouseHovered();
	virtual void MouseUnHovered();

public:
	virtual CItemSlot* Clone();

protected:
	std::weak_ptr<class CImage> mSlotFrame;
	std::weak_ptr<class CImage> mIcon;

	int Count = 1;

public:
	void SetIcon(std::string Name, const TCHAR* Path);
	void AddCount(int N) { Count += N; }
};

