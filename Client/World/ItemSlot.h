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
	virtual void MouseClicked();

public:
	virtual CItemSlot* Clone();

protected:
	std::weak_ptr<class CImage> mSlotFrame;
	std::weak_ptr<class CImage> mIcon;

	std::weak_ptr<class CDataAsset> mDataAsset;

	int Count = 1;

	std::function<void(int /* Index */, std::weak_ptr<CDataAsset> /* Data */)> mOnHovered;
	std::function<void(int /* Index */, std::weak_ptr<CDataAsset> /* Data */)> mOnClicked;

public:
	int Index = -1;
	
	void SetIcon(std::string Name, const TCHAR* Path);
	void SetDataAsset(std::weak_ptr<class CDataAsset> Asset);
	void AddCount(int N) { Count += N; }

	template<typename T>
	void SetOnHoveredFunction(T* Object, void(T::* Func)(int /* Index */, std::weak_ptr<CDataAsset> /* Data */))
	{
		mOnHovered = std::bind(Func, Object, std::placeholders::_1, std::placeholders::_2);
	}

	template<typename T>
	void SetOnClickedFunction(T* Object, void(T::* Func)(int /* Index */, std::weak_ptr<CDataAsset> /* Data */))
	{
		mOnClicked = std::bind(Func, Object, std::placeholders::_1, std::placeholders::_2);
	}
};

