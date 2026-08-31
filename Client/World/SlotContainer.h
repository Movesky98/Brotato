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

public:
	void AddSlotData(std::weak_ptr<class CDataAsset> InAsset);
	void RemoveSlotData(std::weak_ptr<class CDataAsset> InAsset);
	void ClearSlot();

	void SetStackMode(ESlotStackMode Mode) { mStackMode = Mode; }
};

