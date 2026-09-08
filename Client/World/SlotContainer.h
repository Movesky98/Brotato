#pragma once
#include "World/WidgetContainer.h"

enum class ESlotStackMode
{
	Stack,
	Separate,
};

class CSlotContainer : public CWidgetContainer
{
public:
	CSlotContainer();
	CSlotContainer(const CSlotContainer& src);
	virtual ~CSlotContainer();

public:
	virtual bool Init();

public:
	virtual CSlotContainer* Clone();

protected:
	std::unordered_multimap<std::string /* AssetName */, std::weak_ptr<class CItemSlot> /* Slot */> Slots;

	ESlotStackMode mStackMode;

	std::function<void(std::weak_ptr<class CDataAsset>)> mOnSlotHovered;
	std::function<void(std::weak_ptr<class CDataAsset>)> mOnSlotClicked;

	void OnSlotHovered(int Index, std::weak_ptr<class CDataAsset> Data);
	void OnSlotClicked(int Index, std::weak_ptr<class CDataAsset> Data);

public:
	void AddSlotData(std::weak_ptr<class CDataAsset> InAsset);
	void RemoveSlotData(std::weak_ptr<class CDataAsset> InAsset);
	void ClearSlot();

	void SetStackMode(ESlotStackMode Mode) { mStackMode = Mode; }

	template<typename T>
	void SetOnHoveredFunction(T* Object, void(T::* Func)(std::weak_ptr<CDataAsset> /* Data */))
	{
		mOnSlotHovered = std::bind(Func, Object, std::placeholders::_1);
	}

	template<typename T>
	void SetOnClickedFunction(T* Object, void(T::* Func)(std::weak_ptr<CDataAsset> /* Data */))
	{
		mOnSlotClicked = std::bind(Func, Object, std::placeholders::_1);
	}
};