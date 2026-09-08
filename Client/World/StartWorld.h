#pragma once

#include "World/World.h"

class CStartWorld :
    public CWorld
{
public:
	CStartWorld();
	virtual ~CStartWorld();

public:
	virtual bool Init();

protected:
	std::weak_ptr<class CTitleScreen> mTitleScreen;
	std::weak_ptr<class CCharacterSelectScreen> mCharacterSelectScreen;

	void ShowCharacterSelectScreen();
	void ShowTitleScreen();
	void ToMainWorld(std::weak_ptr<class CDataAsset> DataAsset);
};

