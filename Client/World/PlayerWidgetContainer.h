#pragma once
#include "World/WidgetContainer.h"

class CPlayerWidgetContainer :
    public CWidgetContainer
{
public:
	CPlayerWidgetContainer();
	CPlayerWidgetContainer(const CPlayerWidgetContainer& src);
	virtual ~CPlayerWidgetContainer();

public:
	virtual bool Init();

public:
	virtual CPlayerWidgetContainer* Clone();

	virtual void SubscribeDelegates(std::weak_ptr<class CPlayer> InPlayer);
	virtual void UnsubscribeDelegates();

	virtual void SetUp();
	virtual void TearDown();
};

