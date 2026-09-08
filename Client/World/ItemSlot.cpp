#include "ItemSlot.h"

#include "DataAsset.h"

#include "World/Image.h"

CItemSlot::CItemSlot()
{
}

CItemSlot::CItemSlot(const CItemSlot& src)
    : CWidgetContainer(src)
{
}

CItemSlot::~CItemSlot()
{
}

bool CItemSlot::Init()
{
    if (!CWidgetContainer::Init()) return false;

    // Texture Frame, Background Image 설정
    // Size..설정 필요없을 것 같기도 하고 어차피 ItemSlotContainer에서 할거니까
    // Icon은 따로 설정하는 방향으로. 데이터만 넣으면 설정될 수 있게 초기화는 해줘야 함.

    SetSize(64.f, 64.f);

    mSlotFrame = CreateWidget<CImage>("SlotFrame", 1);
    mIcon = CreateWidget<CImage>("Icon", 1);

    auto SlotFrame = mSlotFrame.lock();
    if (SlotFrame)
    {
        SlotFrame->SetSize(64.f, 64.f);
        SlotFrame->SetTexture("SlotFrame", TEXT("Custom/PauseMenu/Outline_Panel.png"));
        SlotFrame->SetTint(FVector4(0.38f, 0.38f, 0.38f, 1.f));
    }

    auto Icon = mIcon.lock();
    if (Icon)
    {
        Icon->SetSize(64.f, 64.f);
        Icon->SetTint(1.f, 1.f, 1.f, 1.f);
    }

    return true;
}

void CItemSlot::MouseHovered()
{
    CWidgetContainer::MouseHovered();

    auto SlotFrame = mSlotFrame.lock();
    auto Icon = mIcon.lock();

    if (!SlotFrame) return;
    if (!Icon) return;

    FVector4 Tint(1.f, 1.f, 1.f, 0.7f);

    SlotFrame->SetTint(Tint);
    Icon->SetTint(Tint);

    if(mOnHovered)
        mOnHovered(Index, mDataAsset);
}

void CItemSlot::MouseUnHovered()
{
    auto SlotFrame = mSlotFrame.lock();
    auto Icon = mIcon.lock();

    if (!SlotFrame) return;
    if (!Icon) return;

    SlotFrame->SetTint(0.38f, 0.38f, 0.38f, 1.f);
    Icon->SetTint(1.f, 1.f, 1.f, 1.f);
}

void CItemSlot::MouseClicked()
{

    if(mOnClicked)
        mOnClicked(Index, mDataAsset);
}

CItemSlot* CItemSlot::Clone()
{
    return new CItemSlot(*this);
}

void CItemSlot::SetIcon(std::string Name, const TCHAR* Path)
{   
    auto Icon = mIcon.lock();
    if (Icon)
    {
        Icon->SetTexture(Name + std::string("Icon"), Path);
    }
}

void CItemSlot::SetDataAsset(std::weak_ptr<class CDataAsset> InAsset)
{
    if (InAsset.expired()) return;

    mDataAsset = InAsset;
    
    auto Asset = InAsset.lock();
    SetIcon(Asset->Name, Asset->TextureInfo.IconPath.c_str());
}
