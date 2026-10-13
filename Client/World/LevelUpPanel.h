#pragma once
#include "World/WidgetContainer.h"

enum class EPrimaryStats;

struct FUpgradeCardDesc
{
	std::weak_ptr<class CItemDetailPanel> mCard;
	std::weak_ptr<class CInputPromptButton> mButton;

	EPrimaryStats mStat;
};

class CLevelUpPanel :
    public CWidgetContainer
{
public:
	CLevelUpPanel();
	CLevelUpPanel(const CLevelUpPanel& src);
	virtual ~CLevelUpPanel();

public:
	virtual bool Init();
	virtual CLevelUpPanel* Clone();

protected:
	FUpgradeCardDesc mUpgradeCards[4];

	std::weak_ptr<class CInputPromptButton> mRerollButton;

	std::function<void(EPrimaryStats /* Stat */)> mOnUpgradeCardSelectedFunc;
	std::function<void()> mOnLevelUpEndedFunc;

	void OnSelectButtonClicked();
	void OnRerollButtonClicked();

public:
	int mCount = 0;

	void RefreshUpgradeCards();

	template<typename T>
	void SetOnUpgradeCardSelectedFunction(T* Obj, void(T::* Func)(EPrimaryStats))
	{
		mOnUpgradeCardSelectedFunc = std::bind(Func, Obj, std::placeholders::_1);
	}

	template<typename T>
	void SetOnLevelUpEndedFunction(T* Obj, void(T::* Func)())
	{
		mOnLevelUpEndedFunc = std::bind(Func, Obj);
	}
};

