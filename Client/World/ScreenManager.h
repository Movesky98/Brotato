#pragma once

#include "EngineInfo.h"
#include "PlayerWidgetContainer.h"

enum class EScreenType
{
	None = -1,
	ShopMenu,
	PauseMenu,
	GameOverScreen,
	MAX
};

class CScreenManager : public std::enable_shared_from_this<CScreenManager>
{
public:
	CScreenManager();
	~CScreenManager();

public:
	bool Init(std::weak_ptr<class CUIManager> InUIManager, std::weak_ptr<class CPlayer> InPlayer);

protected:
	std::weak_ptr<class CUIManager> mUIManager;

	std::weak_ptr<class CImage> mBackgroundImage;
	std::weak_ptr<class CPlayerHUD> mPlayerHUD;
	std::weak_ptr<class CRewardMenu> mRewardMenu;
	std::unordered_map<EScreenType, std::weak_ptr<CPlayerWidgetContainer>> mScreenMap;

	EScreenType mCurScreenType = EScreenType::None;
	EScreenType mPrevScreenType = EScreenType::None;

public:
	bool ShowScreen(EScreenType InType);
	void HideScreen();
	EScreenType GetCurrentScreenType() const { return mCurScreenType; }
	std::weak_ptr<CPlayerWidgetContainer> GetScreen(EScreenType Type) const { return mScreenMap.find(Type)->second; }
	std::weak_ptr<class CRewardMenu> GetRewardMenu() const { return mRewardMenu; }

	void ShowPlayerHUD();
	void ShowPlayerHUD(int Wave, int WaveCount);
	void HidePlayerHUD();

	void ShowRewardMenu(int LevelCount);
	void HideRewardMenu();

	void SetWaveText(int Wave);
	void SetWaveCountText(int WaveCount);
};

