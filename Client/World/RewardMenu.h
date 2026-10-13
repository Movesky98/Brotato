#pragma once
#include "PlayerWidgetContainer.h"

enum class EPrimaryStats;

class CRewardMenu :
    public CPlayerWidgetContainer
{
public:
	CRewardMenu();
	CRewardMenu(const CRewardMenu& src);
	virtual ~CRewardMenu();

public:
	virtual bool Init();
	virtual CRewardMenu* Clone();

protected:
	void SubscribeDelegates(std::weak_ptr<class CPlayer> InPlayer) override;
	void UnsubscribeDelegates() override;

protected:
	std::weak_ptr<class CImage>				mBackgroundImage;
	std::weak_ptr<class CStatPanel>			mStatPanel;
	std::weak_ptr<class CItemFoundPanel>	mItemFoundPanel;
	std::weak_ptr<class CLevelUpPanel>		mLevelUpPanel;

	std::weak_ptr<class CInventoryComponent> mInvComp;

	std::function<void()> mOnLevelUpEndedFunc;

	void OnLevelUpEnded();
	void OnUpgradeCardSelected(EPrimaryStats Stat);

public:
	void ShowLevelUpPanel(int LevelCount);

	template<typename T>
	void SetOnLevelUpEndedFunction(T* Obj, void(T::* Func)())
	{
		mOnLevelUpEndedFunc = std::bind(Func, Obj);
	}
};

