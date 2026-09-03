#include "PauseMenu.h"

#include "World/Image.h"
#include "World/Button.h"
#include "World/TextBlock.h"

#include "StatPanel.h"
#include "DataAsset.h"
#include "SlotContainer.h"
#include "Component/InventoryComponent.h"

#include "DataManager.h"


CPauseMenu::CPauseMenu()
{
}

CPauseMenu::CPauseMenu(const CPauseMenu& src)
    : CWidgetContainer(src)
{
}

CPauseMenu::~CPauseMenu()
{
}

bool CPauseMenu::Init()
{
    if (!CWidgetContainer::Init()) return false;

    mResumeButton = CreateMenuButton("Resume", TEXT("Resume"), FVector2(64.f, 160.f));
    mEndRunButton = CreateMenuButton("EndRun", TEXT("EndRun"), FVector2(64.f, 216.f));
    mRestartButton = CreateMenuButton("Restart", TEXT("Restart"), FVector2(64.f, 272.f));
    mCodexButton = CreateMenuButton("Codex", TEXT("Codex"), FVector2(64.f, 328.f));
    mOptionsButton = CreateMenuButton("Options", TEXT("Options"), FVector2(64.f, 384.f));
    mQuitButton = CreateMenuButton("Quit", TEXT("Quit"), FVector2(64.f, 440.f));
    
    mBackgroundImage = CreateWidget<CImage>("BackgroundImage", 0);
    mStatPanel = CreateWidget<CStatPanel>("StatPanel", 0);

    auto BackgroundImage = mBackgroundImage.lock();
    if (BackgroundImage)
    {
        BackgroundImage->SetPos(0.f, 0.f);
        BackgroundImage->SetSize(1280.f, 720.f);
        BackgroundImage->SetTexture("Pause_BackgroundImage", TEXT("Custom/PauseMenu/Background.png"));
        BackgroundImage->SetOpacity(0.6);
    }

    auto StatPanel = mStatPanel.lock();
    if (StatPanel)
    {
        StatPanel->SetPos(983.f, 54.f);
        StatPanel->SetSize(260.f, 567.f);
    }

    CreateInventorySlotPanel();
    
    return true;
}

CPauseMenu* CPauseMenu::Clone()
{
    return new CPauseMenu(*this);
}

std::weak_ptr<CButton> CPauseMenu::CreateMenuButton(const std::string& Name, const TCHAR* Text, const FVector2& Position)
{
    std::weak_ptr<CButton> TempButton = CreateWidget<CButton>(Name + std::string("Button"), 1);
    std::weak_ptr<CTextBlock> TempTextBlock = CreateWidget<CTextBlock>(Name + std::string("TextBlock"), 1);
    
    auto Button = TempButton.lock();
    auto TextBlock = TempTextBlock.lock();
    if (!Button || !TextBlock) return std::weak_ptr<CButton>();

    Button->SetPos(Position.x, Position.y);
    Button->SetSize(360.f, 40.f);
    Button->SetChild(TextBlock);

    for (int i = 0; i < EWidgetState::End; ++i)
    {
        Button->SetTexture((EWidgetState::Type)i, "PauseMenuButton", TEXT("Custom/PauseMenu/Button1.png"));
    }

    TextBlock->SetSize(360.f, 40.f);
    TextBlock->SetText(Text);
    TextBlock->SetFontSize(32.f);
    TextBlock->SetTextColor(FVector4::White);
    TextBlock->SetAlignH(ETextAlignH::Center);
    TextBlock->SetAlignV(ETextAlignV::Middle);

    return TempButton;
}

void CPauseMenu::CreateInventorySlotPanel()
{
    mWeaponTextBlock = CreateWidget<CTextBlock>("WeaponTextBlock", 1);
    mWeaponSlotContainer = CreateWidget<CSlotContainer>("WeaponSlotContainer", 1);
    mItemTextBlock = CreateWidget<CTextBlock>("ItemTextBlock", 1);
    mItemSlotContainer = CreateWidget<CSlotContainer>("ItemSlotContainer", 1);
    auto ItemHSeparator = CreateWidget<CImage>("ItemHSeparator", 1).lock();
    auto WeaponHSeparator = CreateWidget<CImage>("WeaponHSeparator", 1).lock();

    if (ItemHSeparator && WeaponHSeparator)
    {
        ItemHSeparator->SetPos(539.f, 193.f);
        ItemHSeparator->SetSize(408.f, 3.f);
        ItemHSeparator->SetTint(FVector4(1.f, 1.f, 1.f, 0.2f));

        WeaponHSeparator->SetPos(594.f, 67.f);
        WeaponHSeparator->SetSize(353.f, 3.f);
        WeaponHSeparator->SetTint(FVector4(1.f, 1.f, 1.f, 0.2f));
    }

    auto ItemTextBlock = mItemTextBlock.lock();
    if (ItemTextBlock)
    {
        ItemTextBlock->SetPos(459.f, 172.f);
        ItemTextBlock->SetSize(220.f, 45.f);
        ItemTextBlock->SetFontSize(32.f);
        ItemTextBlock->SetTextColor(FVector4::White);
        ItemTextBlock->SetAlignH(ETextAlignH::Left);
        ItemTextBlock->SetAlignV(ETextAlignV::Middle);
        ItemTextBlock->SetText(TEXT("Item"));
    }

    auto ItemSlotContainer = mItemSlotContainer.lock();
    if (ItemSlotContainer)
    {
        ItemSlotContainer->SetPos(459.f, 228.f);
        ItemSlotContainer->SetSize(488.f, 347.f);
    }

    auto WeaponTextBlock = mWeaponTextBlock.lock();
    if (WeaponTextBlock)
    {
        WeaponTextBlock->SetPos(459.f, 46.f);
        WeaponTextBlock->SetSize(220.f, 45.f);
        WeaponTextBlock->SetFontSize(32.f);
        WeaponTextBlock->SetTextColor(FVector4::White);
        WeaponTextBlock->SetAlignH(ETextAlignH::Left);
        WeaponTextBlock->SetAlignV(ETextAlignV::Middle);
        WeaponTextBlock->SetText(TEXT("Weapon"));
    }

    auto WeaponSlotContainer = mWeaponSlotContainer.lock();
    if (WeaponSlotContainer)
    {
        WeaponSlotContainer->SetPos(459.f, 102.f);
        WeaponSlotContainer->SetSize(488.f, 64.f);
    }
}

void CPauseMenu::UpdateInventory(std::weak_ptr<const class CInventoryComponent> InInventoryComp)
{
    auto InventoryComp = InInventoryComp.lock();
    if (InventoryComp)
    {
        // 우선은 이렇게
        // 나중에 상점 생기고 추가할 수 있게 되면
        // 데이터 전체를 변경하는 방식이 아니라, 추가하는 방식으로 변경할 것
        // mWeaponAssets = InventoryComp->GetWeaponData();
        // mItemAssets = InventoryComp->GetItemData();
        
        // 근데 무기 / 아이템의 데이터에셋화를 아직 진행 안했으니 Enemy 정보가지고 보여줄 필요가 있을 듯?
        mItemAssets = CDataManager::GetInst()->GetEnemiesData();
        mWeaponAssets = mItemAssets;

        auto ItemSlotContainer = mItemSlotContainer.lock();
        if (ItemSlotContainer)
        {
            for (auto& Asset : mItemAssets)
            {
                ItemSlotContainer->AddSlotData(Asset);
            }
        }

        auto WeaponSlotContainer = mWeaponSlotContainer.lock();
        if (WeaponSlotContainer)
        {
            for (auto& Asset : mWeaponAssets)
            {
                WeaponSlotContainer->AddSlotData(Asset);
            }
        }
    }
}
