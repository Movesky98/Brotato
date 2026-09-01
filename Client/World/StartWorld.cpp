#include "StartWorld.h"

#include "StartPlayer.h"

#include "World/UIManager.h"
#include "StartWidget.h"
#include "TitleScreen.h"

CStartWorld::CStartWorld()
{}

CStartWorld::~CStartWorld()
{}

bool CStartWorld::Init()
{
	CWorld::Init();

	auto Player = CreateActor<CStartPlayer>("Player").lock();

	auto UIMgr = GetUIManager().lock();

	if (UIMgr)
	{
		auto MainUI = UIMgr->CreateWidgetContainer<CTitleScreen>("TitleScreen", 0).lock();

		if (MainUI)
		{
			MainUI->SetPos(0.f, 0.f);
		}
	}

	return true;
}
