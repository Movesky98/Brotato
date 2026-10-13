#include "ItemFoundPanel.h"

CItemFoundPanel::CItemFoundPanel()
{
}

CItemFoundPanel::CItemFoundPanel(const CItemFoundPanel& src)
    : CWidgetContainer(src)
{
}

CItemFoundPanel::~CItemFoundPanel()
{
}

bool CItemFoundPanel::Init()
{
    if (!CWidgetContainer::Init()) return false;

    return true;
}

CItemFoundPanel* CItemFoundPanel::Clone()
{
    return new CItemFoundPanel(*this);
}
