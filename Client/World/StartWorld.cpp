#include "StartWorld.h"
#include "MainWorld.h"

#include "TitleScreen.h"
#include "CharacterSelectScreen.h"

#include "World/UIManager.h"
#include "World/WorldManager.h"

CStartWorld::CStartWorld()
{}

CStartWorld::~CStartWorld()
{}

bool CStartWorld::Init()
{
	CWorld::Init();

	auto UIMgr = GetUIManager().lock();

	if (UIMgr)
	{
		mTitleScreen = UIMgr->CreateWidgetContainer<CTitleScreen>("TitleScreen", 0);
		mCharacterSelectScreen = UIMgr->CreateWidgetContainer<CCharacterSelectScreen>("CharacterScreen");

		auto TitleScreen = mTitleScreen.lock();
		if (TitleScreen)
		{
			TitleScreen->SetPos(0.f, 0.f);
			TitleScreen->SetEnable(true);
			TitleScreen->SetOnStartButtonClickedFunction(this, &CStartWorld::ShowCharacterSelectScreen);
		}

		auto CharacterSelectScreen = mCharacterSelectScreen.lock();
		if (CharacterSelectScreen)
		{
			CharacterSelectScreen->SetPos(0.f, 0.f);
			CharacterSelectScreen->SetEnable(false);
			CharacterSelectScreen->SetOnBackButtonClickedFunction(this, &CStartWorld::ShowTitleScreen);
			CharacterSelectScreen->SetOnCharacterSlotClickedFunction(this, &CStartWorld::ToMainWorld);
		}
	}

	return true;
}

void CStartWorld::ShowCharacterSelectScreen()
{
	auto CharacterSelectScreen = mCharacterSelectScreen.lock();
	if (CharacterSelectScreen)
	{
		CharacterSelectScreen->SetPos(0.f, 0.f);
		CharacterSelectScreen->SetEnable(true);
	}

	auto TitleScreen = mTitleScreen.lock();
	if (TitleScreen)
	{
		TitleScreen->SetPos(0.f, 0.f);
		TitleScreen->SetEnable(false);
	}
}

void CStartWorld::ShowTitleScreen()
{
	auto CharacterSelectScreen = mCharacterSelectScreen.lock();
	if (CharacterSelectScreen)
	{
		CharacterSelectScreen->SetPos(0.f, 0.f);
		CharacterSelectScreen->SetEnable(false);
	}

	auto TitleScreen = mTitleScreen.lock();
	if (TitleScreen)
	{
		TitleScreen->SetPos(0.f, 0.f);
		TitleScreen->SetEnable(true);
	}
}

void CStartWorld::ToMainWorld(std::weak_ptr<class CDataAsset> DataAsset)
{
	auto MainWorld = CWorldManager::GetInst()->CreateWorld<CMainWorld>(true).lock();
	if (MainWorld)
	{
		MainWorld->InitializeWorldSetting(DataAsset);
	}
}
