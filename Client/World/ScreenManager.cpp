#include "ScreenManager.h"

#include "PlayerWidgetContainer.h"
#include "PlayerHUD.h"
#include "RewardMenu.h"
#include "ShopMenu.h"
#include "PauseMenu.h"
#include "GameOverScreen.h"

#include "Component/InventoryComponent.h"

#include "World/Image.h"
#include "World/UIManager.h"


CScreenManager::CScreenManager()
{
}

CScreenManager::~CScreenManager()
{
}

bool CScreenManager::Init(std::weak_ptr<class CUIManager> InUIManager, std::weak_ptr<class CPlayer> InPlayer)
{
	if (InUIManager.expired() || InPlayer.expired()) return false;
	
	auto UIManager = InUIManager.lock();
	auto Player = InPlayer.lock();

	mUIManager = UIManager;
	mPlayerHUD = UIManager->CreateWidgetContainer<CPlayerHUD>("PlayerHUD");
	mRewardMenu = UIManager->CreateWidgetContainer<CRewardMenu>("RewardMenu");

	auto PlayerHUD = std::dynamic_pointer_cast<CPlayerWidgetContainer>(mPlayerHUD.lock());
	auto RewardMenu = std::dynamic_pointer_cast<CPlayerWidgetContainer>(mRewardMenu.lock());
	auto ShopMenu = UIManager->CreateWidgetContainer<CShopMenu>("ShopMenu").lock();
	auto PauseMenu = UIManager->CreateWidgetContainer<CPauseMenu>("PauseMenu").lock();
	auto GameOverScreen = UIManager->CreateWidgetContainer<CGameOverScreen>("GameOverScreen").lock();

	if (!PlayerHUD) return false;
	if (!RewardMenu) return false;
	if (!ShopMenu) return false;
	if (!PauseMenu) return false;
	if (!GameOverScreen) return false;

	/*BackgroundImage->SetPos(0.f, 0.f);
	BackgroundImage->SetSize(1280.f, 720.f);
	BackgroundImage->SetTexture("Screen_BackgroundImage", TEXT("Custom/Background.png"));
	BackgroundImage->SetTint(1.f, 1.f, 1.f, 1.f);*/

	mScreenMap.insert(std::make_pair(EScreenType::ShopMenu, ShopMenu));
	mScreenMap.insert(std::make_pair(EScreenType::PauseMenu, PauseMenu));
	mScreenMap.insert(std::make_pair(EScreenType::GameOverScreen, GameOverScreen));

	PlayerHUD->SubscribeDelegates(Player);
	PlayerHUD->SetEnable(true);
	PlayerHUD->SetPos(0.f, 0.f);

	RewardMenu->SubscribeDelegates(Player);
	RewardMenu->SetEnable(false);
	RewardMenu->SetPos(0.f, 0.f);

	for (const auto& [Type, ScreenWeak] : mScreenMap)
	{
		auto Screen = ScreenWeak.lock();
		if (Screen)
		{
			Screen->SubscribeDelegates(Player);
			Screen->SetEnable(false);
			Screen->SetPos(0.f, 0.f);
		}
	}

	return true;
}

bool CScreenManager::ShowScreen(EScreenType InType)
{
	if (mCurScreenType == EScreenType::GameOverScreen) return false;

	for (const auto& [Type, ScreenWeak] : mScreenMap)
	{
		auto Screen = ScreenWeak.lock();
		if (!Screen) continue;

		if (Type != InType)
		{
			Screen->TearDown();
			continue;
		}

		Screen->SetUp();

		if (mCurScreenType != EScreenType::None)
		{
			mPrevScreenType = mCurScreenType;
		}

		mCurScreenType = Type;
	}

	return true;
}

void CScreenManager::HideScreen()
{
	if (mCurScreenType == EScreenType::None) return;

	auto CurrentScreen = mScreenMap.find(mCurScreenType)->second.lock();
	if (CurrentScreen)
	{
		CurrentScreen->TearDown();
	}

	mCurScreenType = EScreenType::None;

	if (mPrevScreenType != EScreenType::None)
	{
		ShowScreen(mPrevScreenType);
		mPrevScreenType = EScreenType::None;
	}
}

void CScreenManager::ShowPlayerHUD()
{
	auto PlayerHUD = mPlayerHUD.lock();
	if (PlayerHUD)
	{
		PlayerHUD->SetEnable(true);
	}
}

void CScreenManager::ShowPlayerHUD(int Wave, int WaveCount)
{
	auto PlayerHUD = mPlayerHUD.lock();
	if (PlayerHUD)
	{
		PlayerHUD->SetEnable(true);
		PlayerHUD->SetWaveText(Wave);
		PlayerHUD->SetWaveCountText(WaveCount);
	}
}

void CScreenManager::HidePlayerHUD()
{
	auto PlayerHUD = mPlayerHUD.lock();
	if (PlayerHUD)
	{
		PlayerHUD->SetEnable(true);
	}
}

void CScreenManager::ShowRewardMenu(int LevelCount)
{
	auto RewardMenu = mRewardMenu.lock();
	if (RewardMenu)
	{
		RewardMenu->SetEnable(true);
		RewardMenu->ShowLevelUpPanel(LevelCount);
	}
}

void CScreenManager::HideRewardMenu()
{
	auto RewardMenu = mRewardMenu.lock();
	if (RewardMenu)
	{
		RewardMenu->SetEnable(false);
	}
}

void CScreenManager::SetWaveCountText(int WaveCount)
{
	auto PlayerHUD = mPlayerHUD.lock();
	if (PlayerHUD)
	{
		PlayerHUD->SetWaveCountText(WaveCount);
	}
}
