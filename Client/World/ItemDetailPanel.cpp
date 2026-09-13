#include "ItemDetailPanel.h"

#include "StatLine.h"
#include "StatTable.h"
#include "ItemData.h"
#include "ConvertString.h"

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

void CItemDetailPanel::CreateStatLines(const std::vector<FStatLineDesc>& LineDescs)
{
    // 있던거 다 지움
    for (auto& StatLineWeak : mStatLines)
    {
        auto StatLine = StatLineWeak.lock();
        if (StatLine)
        {
            StatLine->Destory();
        }
    }

    mStatLines.clear();

    // 원본 상점 카드 기준 배치 (패널 좌상단 기준)
    FVector2 AreaPos(9.f, 82.f);
    float RowW = 226.f;
    float RowH = 24.f;

    for (size_t i = 0; i < LineDescs.size(); i++)
    {
        std::shared_ptr<CStatLine> NewStatLine = CreateWidget<CStatLine>("StatLine", 0).lock();
        if (!NewStatLine) continue;

        NewStatLine->SetPos(AreaPos.x, AreaPos.y + i * RowH);
        NewStatLine->SetSize(RowW, RowH);

        NewStatLine->SetIconPos(0.f, 5.f);
        NewStatLine->SetIconSize(14.f, 14.f);

        NewStatLine->SetValuePos(17.f, 0.f);
        NewStatLine->SetValueSize(28.f, RowH);
        NewStatLine->SetValueFontSize(14.f);

        NewStatLine->SetNamePos(45.f, 0.f);
        NewStatLine->SetNameSize(181.f, RowH);
        NewStatLine->SetNameFontSize(14.f);

        NewStatLine->Initialize(LineDescs[i]);

        mStatLines.push_back(NewStatLine);
    }
}

void CItemDetailPanel::SetShopDataDesc(std::weak_ptr<class CShopData> Data)
{
    auto ShopData = Data.lock();
    if (!ShopData) return;

    auto Icon = mIcon.lock();
    if (Icon)
    {
        Icon->SetTexture(ShopData->Name + std::string("_Icon"), ShopData->TextureInfo.IconPath.c_str());
    }

    auto NameText = mNameText.lock();
    if (NameText)
    {
        NameText->SetText(ToWString(ShopData->Name).c_str());
    }

    std::vector<FStatLineDesc> LineDescs;

    // 무기 스탯 줄은 이후 CWeaponStatLine으로 채운다. 지금은 줄 없이 비워둔다.
    if (ShopData->AssetType == EDataAssetType::Item)
    {
        auto ItemData = std::dynamic_pointer_cast<CItemData>(ShopData);
        if (ItemData)
        {
            const FStat& Stats = ItemData->Stats;

            for (int i = 0; i < (int)EPrimaryStats::MAX; i++)
            {
                // 0이 아닐 경우에만 저장
                if (Stats.Primary[i])
                {
                    FStatLineDesc NewDesc;

                    NewDesc.Name = GetStatName((EPrimaryStats)i);
                    NewDesc.IconPath = GetStatIcon((EPrimaryStats)i);
                    NewDesc.Value = Stats.Primary[i];

                    LineDescs.push_back(NewDesc);
                }
            }

            for (int i = 0; i < (int)ESecondaryStats::MAX; i++)
            {
                // 0이 아닐 경우에만 저장
                if (Stats.Secondary[i])
                {
                    FStatLineDesc NewDesc;

                    NewDesc.Name = GetStatName((ESecondaryStats)i);
                    NewDesc.IconPath = GetStatIcon((ESecondaryStats)i);
                    NewDesc.Value = Stats.Secondary[i];

                    LineDescs.push_back(NewDesc);
                }
            }
        }
    }

    CreateStatLines(LineDescs);
}
