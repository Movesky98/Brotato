#pragma once
#include "PlayerWidgetContainer.h"
class CPlayerHUD :
    public CPlayerWidgetContainer
{
public:
	CPlayerHUD();
	CPlayerHUD(const CPlayerHUD& src);
	virtual ~CPlayerHUD();

public:
	virtual bool Init();
	virtual CPlayerHUD* Clone();

protected:
	void SubscribeDelegates(std::weak_ptr<class CPlayer> InPlayer) override;
	void UnsubscribeDelegates() override;

protected:
	std::weak_ptr<class CTextBlock> mWaveText;
	std::weak_ptr<class CTextBlock> mWaveCountText;
	
	std::weak_ptr<class CProgressBar> mLifeBar;
	std::weak_ptr<class CTextBlock> mLifeText;

	std::weak_ptr<class CProgressBar> mXPBar;
	std::weak_ptr<class CTextBlock> mXPText;

	std::weak_ptr<class CTextBlock> mMaterialText;

	void OnUpdatedXP(int CurrentXP, int RequiredXP);
	void SetLifeBarPercent(const float Percent);
	void SetXPBarPercent(const float Percent);
	
public:
	void SetWaveText(int Wave);
	void SetWaveCountText(int Count);

	void SetLifeText(float CurrentHP, float MaxHP);
	void SetXPText(int XPLevel);

	void SetMaterialText(int Material);
};

