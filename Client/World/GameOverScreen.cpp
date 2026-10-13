#include "GameOverScreen.h"

#include "Player.h"
#include "Component/InventoryComponent.h"
#include "SlotContainer.h"
#include "StatPanel.h"

#include "World/Image.h"
#include "World/Button.h"
#include "World/TextBlock.h"

#include "World/WorldManager.h"
#include "StartWorld.h"
#include "MainWorld.h"
#include "CharacterData.h"

CGameOverScreen::CGameOverScreen()
{
}

CGameOverScreen::CGameOverScreen(const CGameOverScreen& src)
    : CPlayerWidgetContainer(src)
{
}

CGameOverScreen::~CGameOverScreen()
{
}

bool CGameOverScreen::Init()
{
    if (!CPlayerWidgetContainer::Init()) return false;

    mBackgroundImage = CreateWidget<CImage>("BackgroundImage");
    mRestartButton = CreateButton("Restart", TEXT("Restart"), FVector2(192.f, 656.f), FVector2(223.f, 43.f), true);
	mNewRunButton = CreateButton("NewRun", TEXT("NewRun"), FVector2(432.f, 656.f), FVector2(223.f, 43.f));
	mReturnToTitleScreenButton = CreateButton("ReturnToTitleScreen", TEXT("Return To Title Screen"), FVector2(686.f, 656.f), FVector2(400.f, 43.f), true);

	mBackgroundPanel = CreateWidget<CImage>("BackgroundPanel");
	mStatPanel = CreateWidget<CStatPanel>("StatPanel");
	mWeaponSlotContainer = CreateWidget<CSlotContainer>("WeaponSlotContainer");
	mItemSlotContainer = CreateWidget<CSlotContainer>("ItemSlotContainer");

	if (mBackgroundImage.expired()) return false;
	if (mBackgroundPanel.expired()) return false;
	if (mStatPanel.expired()) return false;
	if (mWeaponSlotContainer.expired()) return false;
	if (mItemSlotContainer.expired()) return false;

	auto RestartButton = mRestartButton.lock();
	if (RestartButton)
	{
		RestartButton->SetWidgetEventFunc(EWidgetEventState::Clicked, this, &CGameOverScreen::ToMainWorld);
	}

	auto ReturnToTitleScreenButton = mReturnToTitleScreenButton.lock();
	if (ReturnToTitleScreenButton)
	{
		ReturnToTitleScreenButton->SetWidgetEventFunc(EWidgetEventState::Clicked, this, &CGameOverScreen::ToStartWorld);
	}

	auto BackgroundImage = mBackgroundImage.lock();
	
	BackgroundImage->SetPos(0.f, 0.f);
	BackgroundImage->SetSize(1280.f, 720.f);
	BackgroundImage->SetTexture("GameOver_BackgroundImage", TEXT("Custom/GameOverScreen/Background.png"));
	BackgroundImage->SetTint(1.f, 1.f, 1.f, 1.f);

	CreateResultPanel();

    return true;
}

CGameOverScreen* CGameOverScreen::Clone()
{
    return new CGameOverScreen(*this);
}

void CGameOverScreen::InitializeWithInventory(std::weak_ptr<class CInventoryComponent> InvCompWeak)
{
	auto InvComp = InvCompWeak.lock();
	if (InvComp)
	{
		InvComp->SetUpdatedAssetsFunction(this, &CGameOverScreen::UpdateResultPanel);
		mPlayerInvComp = InvComp;
	}
}

void CGameOverScreen::UpdateResultPanel()
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

	auto StatPanel = mStatPanel.lock();
	if (StatPanel)
	{
		StatPanel->UpdateStats(InvComp->GetStat());
	}
}

std::weak_ptr<CButton> CGameOverScreen::CreateButton(const std::string& Name, const TCHAR* Text, const FVector2& Position, const FVector2& Size, bool IsActive)
{
    std::weak_ptr<CButton> TempButton = CreateWidget<CButton>(Name + std::string("Button"), 1);
	std::weak_ptr<CTextBlock> TempTextBlock = CreateWidget<CTextBlock>(Name + std::string("TextBlock"), 1);

	auto Button = TempButton.lock();
	auto TextBlock = TempTextBlock.lock();
	if (!Button || !TextBlock) return std::weak_ptr<CButton>();

	Button->SetPos(Position.x, Position.y);
	Button->SetSize(Size.x, Size.y);
	Button->SetChild(TextBlock);

	for (int i = 0; i < EWidgetState::End; ++i)
	{
		Button->SetTexture((EWidgetState::Type)i, "GameOverScreenButton", TEXT("Custom/GameOverScreen/Button.png"));

		Button->SetTint((EWidgetState::Type)i, FVector4(0.f, 0.f, 0.f, 1.f));
	}

	if (IsActive)
	{
		Button->SetTint(EWidgetState::Hovered, FVector4(1.f, 1.f, 1.f, 1.f));
	}

	TextBlock->SetSize(Size.x, Size.y);
	TextBlock->SetText(Text);
	TextBlock->SetFontSize(32.f);
	TextBlock->SetTextColor(FVector4::White);
	TextBlock->SetAlignH(ETextAlignH::Center);
	TextBlock->SetAlignV(ETextAlignV::Middle);

	return TempButton;
}

void CGameOverScreen::CreateResultPanel()
{
	auto BackgroundPanel = mBackgroundPanel.lock();
	auto StatPanel = mStatPanel.lock();
	auto ItemSlotContainer = mItemSlotContainer.lock();
	auto WeaponSlotContainer = mWeaponSlotContainer.lock();
	
	BackgroundPanel->SetPos(53.3f, 68.f);
	BackgroundPanel->SetSize(1173.3f, 570.7f);
	BackgroundPanel->SetTint(0.f, 0.f, 0.f, 0.7f);

	StatPanel->SetPos(56.7f, 71.3f);
	StatPanel->SetSize(266.7f, 564.f);

	WeaponSlotContainer->SetPos(347.3f, 139.3f);
	WeaponSlotContainer->SetSize(854.7f, 70.f);
	WeaponSlotContainer->SetStackMode(ESlotStackMode::Separate);


	auto WeaponText = CreateWidget<CTextBlock>("ItemSlotText").lock();
	auto WeaponSeperator = CreateWidget<CImage>("WeaponSeperator").lock();
	if (WeaponText && WeaponSeperator)
	{
		WeaponText->SetPos(347.3f, 92.7f);
		WeaponText->SetSize(49.f, 30.f);
		WeaponText->SetText(TEXT("무기"));
		WeaponText->SetTextColor(FVector4::White);
		WeaponText->SetAlignH(ETextAlignH::Center);
		WeaponText->SetAlignV(ETextAlignV::Middle);

		WeaponSeperator->SetPos(399.f, 105.f);
		WeaponSeperator->SetSize(803.f, 5.33f);
		WeaponSeperator->SetTint(1.f, 1.f, 1.f, 0.3f);
	}

	ItemSlotContainer->SetPos(347.3f, 289.3f);
	ItemSlotContainer->SetSize(854.7f, 282.f);
	ItemSlotContainer->SetStackMode(ESlotStackMode::Stack);

	auto ItemText = CreateWidget<CTextBlock>("ItemSlotText").lock();
	auto ItemSeperator = CreateWidget<CImage>("ItemSeperator").lock();
	if (ItemText && ItemSeperator)
	{
		ItemText->SetPos(347.3f, 242.7f);
		ItemText->SetSize(74.f, 30.f);
		ItemText->SetText(TEXT("아이템"));
		ItemText->SetTextColor(FVector4::White);
		ItemText->SetAlignH(ETextAlignH::Center);
		ItemText->SetAlignV(ETextAlignV::Middle);

		ItemSeperator->SetPos(424.f, 255.f);
		ItemSeperator->SetSize(596.6f, 5.33f);
		ItemSeperator->SetTint(1.f, 1.f, 1.f, 0.3f);
	}
}

void CGameOverScreen::ToMainWorld()
{
	auto CurrentWorld = std::dynamic_pointer_cast<CMainWorld>(mWorld.lock());
	if (!CurrentWorld) return;

	auto NextWorld = CWorldManager::GetInst()->CreateWorld<CMainWorld>(true).lock();
	if (NextWorld)
	{
		NextWorld->InitializeWorldSetting(CurrentWorld->GetPlayerData());
	}
}

void CGameOverScreen::ToStartWorld()
{
	CWorldManager::GetInst()->CreateWorld<CStartWorld>(true);
}

void CGameOverScreen::SubscribeDelegates(std::weak_ptr<class CPlayer> InPlayer)
{
	auto Player = InPlayer.lock();
	if (!Player) return;

	auto InvComp = Player->GetInventoryComponent().lock();
	if (!InvComp) return;

	mPlayerInvComp = InvComp;

	InvComp->SetUpdatedAssetsFunction(this, &CGameOverScreen::UpdateResultPanel);

	auto StatPanel = mStatPanel.lock();
	if (StatPanel)
	{
		InvComp->SetOnStatUpdatedFunction(StatPanel.get(), &CStatPanel::UpdateStats);
		InvComp->SetOnUpdatedLevelFunction(StatPanel.get(), &CStatPanel::UpdateLevel);
	}
}

void CGameOverScreen::UnsubscribeDelegates()
{
	// 델리게이트 Remove 기능 없음.
	// 추가하고 넣어야 할 듯.
}
