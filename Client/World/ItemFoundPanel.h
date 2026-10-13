#pragma once
#include "World/WidgetContainer.h"

class CItemFoundPanel :
    public CWidgetContainer
{
public:
	CItemFoundPanel();
	CItemFoundPanel(const CItemFoundPanel& src);
	virtual ~CItemFoundPanel();

public:
	virtual bool Init();
	virtual CItemFoundPanel* Clone();
};

