#include "ItemDetailPanel.h"

#include "World/Image.h"
#include "World/TextBlock.h"

CItemDetailPanel::CItemDetailPanel()
{
}

CItemDetailPanel::CItemDetailPanel(const CItemDetailPanel& src)
    : CWidgetContainer(src)
{
}

CItemDetailPanel::~CItemDetailPanel()
{
}

bool CItemDetailPanel::Init()
{
    if (!CWidgetContainer::Init()) return false;

    mIcon = CreateWidget<CImage>("IconImage", 0);
    mNameText = CreateWidget<CTextBlock>("NameTextBlock", 0);

    auto Icon = mIcon.lock();
    if (Icon)
    {
        Icon->SetPos(10.f, 12.f);
        Icon->SetSize(64.f, 64.f);
    }

    auto NameText = mNameText.lock();
    if (NameText)
    {
        NameText->SetPos(88.f, 22.f);
        NameText->SetSize(137.f, 26.f);
        NameText->SetFontSize(20.f);
        NameText->SetTextColor(FVector4::White);
        NameText->SetText(TEXT("Item Name"));
    }


    return true;
}

CItemDetailPanel* CItemDetailPanel::Clone()
{
    return new CItemDetailPanel(*this);
}

void CItemDetailPanel::SetIconTexture(std::string Name, const TCHAR* Path)
{
    auto Icon = mIcon.lock();
    if (Icon)
    {
        Icon->SetTexture(Name, Path);
    }
}

void CItemDetailPanel::SetNameText(const TCHAR* Name)
{
    auto NameText = mNameText.lock();
    if (NameText)
    {
        NameText->SetText(Name);
    }
}
