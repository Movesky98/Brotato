#pragma once
#include "World/WidgetContainer.h"
class CPlayerHUD :
    public CWidgetContainer
{
public:
	CPlayerHUD();
	CPlayerHUD(const CPlayerHUD& src);
	virtual ~CPlayerHUD();

public:
	virtual bool Init();
	virtual CPlayerHUD* Clone();

protected:
	std::weak_ptr<class CTextBlock> mWaveText;
	std::weak_ptr<class CTextBlock> mWaveCountText;
	
	std::weak_ptr<class CProgressBar> mLifeBar;
	std::weak_ptr<class CTextBlock> mLifeText;

	std::weak_ptr<class CProgressBar> mXPBar;
	std::weak_ptr<class CTextBlock> mXPText;

	void SetLifeBarPercent(const float Percent);
	
public:
	void SetWaveText(int Wave);
	void SetWaveCountText(int Count);

	void SetLifeText(float CurrentHP, float MaxHP);
	void SetXPText(int XPLevel);

};

