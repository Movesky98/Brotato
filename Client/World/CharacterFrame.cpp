#include "CharacterFrame.h"

#include "SlotContainer.h"
#include "StatLine.h"

#include "CharacterData.h"
#include "WeaponData.h"
#include "StatTable.h"
#include "ConvertString.h"

#include "World/Image.h"
#include "World/TextBlock.h"


CCharacterFrame::CCharacterFrame()
{
}

CCharacterFrame::CCharacterFrame(const CCharacterFrame& src)
    : CWidgetContainer(src)
{
}

CCharacterFrame::~CCharacterFrame()
{
}

bool CCharacterFrame::Init()
{
    if (!CWidgetContainer::Init()) return false;

    SetSize(512.f, 336.f);

    mMainFrame = CreateWidget<CImage>("MainFrame");
    mPreviewFrame = CreateWidget<CImage>("PreviewFrame", 1);
    mStartWeaponFrame = CreateWidget<CImage>("StartWeaponFrame");
    mStartWeaponSlotContainer = CreateWidget<CSlotContainer>("StartWeaponSlotContainer");
    mGroundImage = CreateWidget<CImage>("GroundImage");
    mNameText = CreateWidget<CTextBlock>("NameText");
    mCharacterFaceImage = CreateWidget<CImage>("CharacterFaceImage");
    mCharacterLegImage = CreateWidget<CImage>("CharacterLegImage");

    auto MainFrame = mMainFrame.lock();
    if (MainFrame)
    {
        MainFrame->SetPos(0.f, 0.f);
        MainFrame->SetSize(512.f, 336.f);
        MainFrame->SetTexture("CharacterFrame_MainFrame", TEXT("Custom/CharacterSelectScreen/Frame_Character.png"));
        MainFrame->SetTint(FVector4::White);
    }

    auto NameText = mNameText.lock();
    if (NameText)
    {
        NameText->SetPos(16.f, 8.f);
        NameText->SetSize(480.f, 48.f);
        NameText->SetText(TEXT("캐릭터 이름"));
        NameText->SetTextColor(FVector4::White);
        NameText->SetAlignH(ETextAlignH::Center);
        NameText->SetAlignV(ETextAlignV::Middle);
        NameText->SetFontSize(36.f);
    }

    auto PreviewFrame = mPreviewFrame.lock();
    auto GroundImage = mGroundImage.lock();
    if (PreviewFrame)
    {
        PreviewFrame->SetPos(16.f, 64.f);
        PreviewFrame->SetSize(176.f, 192.f);
        PreviewFrame->SetTexture("CharacterFrame_PreviewFrame", TEXT("Custom/CharacterSelectScreen/Frame_Preview.png"));
        PreviewFrame->SetTint(FVector4::Black);

        GroundImage->SetPos(16.f, 64.f);
        GroundImage->SetSize(176.f, 192.f);
        GroundImage->SetTexture("CharacterFrame_GroundImage", TEXT("Custom/CharacterSelectScreen/Preview_Ground_1.png"));
    }

    auto StartWeaponFrame = mStartWeaponFrame.lock();
    auto StartWeaponSlotConatiner = mStartWeaponSlotContainer.lock();
    if (StartWeaponFrame)
    {
        StartWeaponFrame->SetPos(16.f, 264.f);
        StartWeaponFrame->SetSize(176.f, 64.f);
        StartWeaponFrame->SetTexture("CharacterFrame_StartWeaponFrame", TEXT("Custom/CharacterSelectScreen/Frame_StartWeapon.png"));
        StartWeaponFrame->SetTint(FVector4::Black);

        StartWeaponSlotConatiner->SetPos(16.f, 264.f);
        StartWeaponSlotConatiner->SetSize(176.f, 64.f);
    }

    auto CharacterFace = mCharacterFaceImage.lock();
    auto CharacterLeg = mCharacterLegImage.lock();
    if (CharacterFace && CharacterLeg)
    {
        CharacterFace->SetPos(55.f, 105.f);
        CharacterFace->SetSize(96.f, 96.f);
        CharacterFace->SetTexture("CharacterFrame_BaseCharacterFace", TEXT("Custom/CharacterSelectScreen/Characters/Engineer.png"));

        CharacterLeg->SetPos(55.f, 171.f);
        CharacterLeg->SetSize(100.f, 50.f);
        CharacterLeg->SetTexture("ChracterFrame_CharacterLeg", TEXT("Custom/legs.png"));
    }

    return true;
}

CCharacterFrame* CCharacterFrame::Clone()
{
    return new CCharacterFrame(*this);
}

void CCharacterFrame::CreateStatLines(std::vector<std::weak_ptr<class CStatLine>>& OutLines, const std::vector<FStatLineDesc> LineDescs)
{
    // 있던거 다 지움
    if (OutLines.size())
    {
        auto iter = OutLines.begin();
        auto iterEnd = OutLines.end();

        for (; iter != iterEnd; )
        {
            auto StatLine = iter->lock();
            if (StatLine)
            {
                iter = OutLines.erase(iter);
                iterEnd = OutLines.end();

                StatLine->Destory();
            }            
        }
    }

    FVector2 AreaPos(208.f, 64.f);
    FVector2 AreaSize(288.f, 264.f);
    float RowH = 23.f;
    float StartYPos = AreaPos.y + (AreaSize.y - LineDescs.size() * RowH) / 2;
    int Count = 0;
    for (const auto Desc : LineDescs)
    {
        std::shared_ptr<CStatLine> NewStatLine = CreateWidget<CStatLine>("StatRow", 1).lock();
        if (NewStatLine)
        {
            NewStatLine->SetPos(AreaPos.x, StartYPos + Count * RowH);
            NewStatLine->SetSize(AreaSize.x, RowH);

            NewStatLine->Initlialize(Desc);

            OutLines.push_back(NewStatLine);
        }

        Count++;
    }
}

void CCharacterFrame::SetMainFrameTint(FVector4 Tint)
{
    auto MainFrame = mMainFrame.lock();
    if (MainFrame)
    {
        MainFrame->SetTint(Tint);
    }
}

void CCharacterFrame::SetCharacterDesc(std::weak_ptr<class CDataAsset> DataAsset)
{
    auto CharacterData = std::dynamic_pointer_cast<CCharacterData>(DataAsset.lock());
    if (!CharacterData) return;

    auto CharacterFace = mCharacterFaceImage.lock();
    if (CharacterFace)
    {
        CharacterFace->SetTexture(CharacterData->Name, CharacterData->TextureInfo.Path.c_str());
    }

    auto NameText = mNameText.lock();
    if (NameText)
    {
        NameText->SetText(ToWString(CharacterData->Name).c_str());
    }

    auto StartWeaponSlotContainer = mStartWeaponSlotContainer.lock();
    if (StartWeaponSlotContainer)
    {
        StartWeaponSlotContainer->ClearSlot();

        StartWeaponSlotContainer->AddSlotData(CharacterData->StartWeapon);
    }

    const FStat Stats = CharacterData->Stats;
    std::vector<FStatLineDesc> LineDescs;

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

    CreateStatLines(mStatLines, LineDescs);
}