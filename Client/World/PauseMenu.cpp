#include "PauseMenu.h"

#include "World/Image.h"
#include "World/Button.h"
#include "World/TextBlock.h"

#include "StatPanel.h"
#include "SlotContainer.h"

#include "MainWorld.h"
#include "Player.h"
#include "Component/InventoryComponent.h"

#include "DataManager.h"


CPauseMenu::CPauseMenu()
{
}

CPauseMenu::CPauseMenu(const CPauseMenu& src)
    : CPlayerWidgetContainer(src)
{
}

CPauseMenu::~CPauseMenu()
{
}

bool CPauseMenu::Init()
{
    if (!CPlayerWidgetContainer::Init()) return false;

    mResumeButton = CreateMenuButton("Resume", TEXT("Resume"), FVector2(64.f, 160.f));
    mEndRunButton = CreateMenuButton("EndRun", TEXT("EndRun"), FVector2(64.f, 216.f));
    mRestartButton = CreateMenuButton("Restart", TEXT("Restart"), FVector2(64.f, 272.f));
    mCodexButton = CreateMenuButton("Codex", TEXT("Codex"), FVector2(64.f, 328.f));
    mOptionsButton = CreateMenuButton("Options", TEXT("Options"), FVector2(64.f, 384.f));
    mQuitButton = CreateMenuButton("Quit", TEXT("Quit"), FVector2(64.f, 440.f));
    
    mBackgroundImage = CreateWidget<CImage>("BackgroundImage", 0);
    mBackgroundPanelImage = CreateWidget<CImage>("BackgroundPanelImage", 1);
    mStatPanel = CreateWidget<CStatPanel>("StatPanel", 1);

    auto BackgroundImage = mBackgroundImage.lock();
    if (BackgroundImage)
    {
        BackgroundImage->SetPos(0.f, 0.f);
        BackgroundImage->SetSize(1280.f, 720.f);
        BackgroundImage->SetTexture("Pause_BackgroundImage", TEXT("Custom/PauseMenu/Background.png"));
    }

    auto BackgroundPanelImage = mBackgroundPanelImage.lock();
    if (BackgroundPanelImage)
    {
        BackgroundPanelImage->SetPos(0.f, 0.f);
        BackgroundPanelImage->SetSize(1280.f, 720.f);
        BackgroundPanelImage->SetTexture("Pause_BackgroundPanelImage", TEXT("Custom/PauseMenu/BackgroundPanel.png"));
        BackgroundPanelImage->SetOpacity(0.6);
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

void CPauseMenu::SetUp()
{
    CPlayerWidgetContainer::SetUp();

    auto MainWorld = std::dynamic_pointer_cast<CMainWorld>(mWorld.lock());
    if (!MainWorld) return;

    auto BackgroundImage = mBackgroundImage.lock();
    if (!BackgroundImage) return;

    if (MainWorld->GetStage() == EGameplayStage::Wave)
        BackgroundImage->SetEnable(false);
    else
        BackgroundImage->SetEnable(true);
}

void CPauseMenu::SubscribeDelegates(std::weak_ptr<class CPlayer> InPlayer)
{
	auto Player = InPlayer.lock();
    if (!Player) return;

	auto InvComp = Player->GetInventoryComponent().lock();
    if (!InvComp) return;
	
    mPlayerInvComp = InvComp;
    InvComp->SetUpdatedAssetsFunction(this, &CPauseMenu::UpdateInventory);
    
    auto StatPanel = mStatPanel.lock();
    if (StatPanel)
    {
        InvComp->SetOnStatUpdatedFunction(StatPanel.get(), &CStatPanel::UpdateStats);
    }
}

void CPauseMenu::UnsubscribeDelegates()
{
}

std::weak_ptr<CButton> CPauseMenu::CreateMenuButton(const std::string& Name, const TCHAR* Text, const FVector2& Position)
{
    std::weak_ptr<CButton> TempButton = CreateWidget<CButton>(Name + std::string("Button"), 2);
    std::weak_ptr<CTextBlock> TempTextBlock = CreateWidget<CTextBlock>(Name + std::string("TextBlock"), 2);
    
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
    mWeaponTextBlock = CreateWidget<CTextBlock>("WeaponTextBlock", 2);
    mWeaponSlotContainer = CreateWidget<CSlotContainer>("WeaponSlotContainer", 2);
    mItemTextBlock = CreateWidget<CTextBlock>("ItemTextBlock", 2);
    mItemSlotContainer = CreateWidget<CSlotContainer>("ItemSlotContainer", 2);
    auto ItemHSeparator = CreateWidget<CImage>("ItemHSeparator", 2).lock();
    auto WeaponHSeparator = CreateWidget<CImage>("WeaponHSeparator", 2).lock();

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

void CPauseMenu::UpdateInventory()
{
    auto ItemSlotContainer = mItemSlotContainer.lock();
    auto WeaponSlotContainer = mWeaponSlotContainer.lock();
    if (!ItemSlotContainer || !WeaponSlotContainer) return;

    auto InvComp = mPlayerInvComp.lock();
    if (!InvComp) return;

    // 추가하기 전에 child 제거 필요함.
    ItemSlotContainer->ClearSlot();
    WeaponSlotContainer->ClearSlot();

    auto Items = InvComp->GetItemData();
    auto Weapons = InvComp->GetWeaponData();

    for (auto Item : Items)
    {
        ItemSlotContainer->AddSlotData(Item);
    }

    for (auto Weapon : Weapons)
    {
        WeaponSlotContainer->AddSlotData(Weapon);
    }
}
