#include "PlayerWidgetContainer.h"

CPlayerWidgetContainer::CPlayerWidgetContainer()
{

}

CPlayerWidgetContainer::CPlayerWidgetContainer(const CPlayerWidgetContainer& src)
    : CWidgetContainer(src)
{

}

CPlayerWidgetContainer::~CPlayerWidgetContainer()
{

}

bool CPlayerWidgetContainer::Init()
{
    if (!CWidgetContainer::Init()) return false;
    
    return true;
}

CPlayerWidgetContainer* CPlayerWidgetContainer::Clone()
{
    return new CPlayerWidgetContainer(*this);
}

void CPlayerWidgetContainer::SubscribeDelegates(std::weak_ptr<class CPlayer> InPlayer)
{


}

void CPlayerWidgetContainer::UnsubscribeDelegates()
{
}

void CPlayerWidgetContainer::SetUp()
{
    SetEnable(true);
}

void CPlayerWidgetContainer::TearDown()
{
    SetEnable(false);
}
