#include "SlotContainer.h"

#include "ItemSlot.h"
#include "DataAsset.h"

#include "World/World.h"
#include "World/UIManager.h"

CSlotContainer::CSlotContainer()
{

}

CSlotContainer::CSlotContainer(const CSlotContainer& src)
    : CWidgetContainer(src)
{
}

CSlotContainer::~CSlotContainer()
{
}

bool CSlotContainer::Init()
{
    if (!CWidgetContainer::Init()) return false;



    return true;
}

CSlotContainer* CSlotContainer::Clone()
{
    return new CSlotContainer(*this);
}

void CSlotContainer::AddSlotData(std::weak_ptr<class CDataAsset> InAsset)
{
    auto Asset = InAsset.lock();
    if (!Asset) return;

    auto iter = Slots.find(Asset->Name);
    
    // 이미 있는 경우
    if (iter != Slots.end() && mStackMode == ESlotStackMode::Stack)
    {
        auto Slot = iter->second.lock();
        if (Slot)
        {
            Slot->AddCount(1);
        }

        return;
    }

    auto World = mWorld.lock();
    if (World)
    {
        auto UIManager = World->GetUIManager().lock();
        if (UIManager)
        {
            auto Slot = CreateWidget<CItemSlot>("ItemSlot", 0).lock();
            if (Slot)
            {
                // 위치 정해주고
                // 아이콘 넣어주고
                
                size_t Count = Slots.size();
                FVector2 ContainerSize = FVector2(GetSize().x, GetSize().y);
                FVector3 DrawPos;

                // 한 행에 들어갈 수 있는 슬롯 최대 수
                int RowCount = (int)(ContainerSize.x / 65.f);

                DrawPos.x = (Count % RowCount) * 65.f;
                DrawPos.y = (Count / RowCount) * 65.f;

                Slot->SetPos(DrawPos);
                Slot->SetIcon(Asset->Name, Asset->TextureInfo.IconPath.c_str());
                Slots.insert(std::pair(Asset->Name, Slot));
            }
        }
    }
}

void CSlotContainer::RemoveSlotData(std::weak_ptr<class CDataAsset> InAsset)
{
    

}

void CSlotContainer::ClearSlot()
{
    auto iter = Slots.begin();
    auto iterEnd = Slots.end();
    
    for (; iter != iterEnd;)
    {
        auto Slot = iter->second.lock();
        if (Slot)
        {
            Slot->Destory();
        }

        iter++;
    }

    Slots.clear();
}
