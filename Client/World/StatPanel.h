#pragma once
#include "World/WidgetContainer.h"
#include "GameTypes.h"

class CImage;
class CButton;
class CTextBlock;

class CStatPanel :
    public CWidgetContainer
{
public:
	CStatPanel();
	CStatPanel(const CStatPanel& src);
	virtual ~CStatPanel();

public:
	virtual bool Init();

public:
	virtual CStatPanel* Clone();

protected:
	std::vector<std::weak_ptr<class CStatRow>> mPrimaryStatRows;
	std::vector<std::weak_ptr<class CStatRow>> mSecondaryStatRows;

	void InitializeStats();
	void CreateStatRows(std::vector<std::weak_ptr<class CStatRow>>& OutRows, bool IsPrimary);
	
	std::weak_ptr<CImage> mBackgroundImage;
	std::weak_ptr<CButton> mPrimaryButton;
	std::weak_ptr<CButton> mSecondaryButton;

	bool mIsShowPrimaryStat = true;

	std::weak_ptr<CButton> CreateButton(const std::string& Name, const TCHAR* Text, const FVector2& Position);

	void ShowPrimaryStat();
	void ShowSecondaryStat();

public:
	void UpdateStats(const FStat Stat);
};

