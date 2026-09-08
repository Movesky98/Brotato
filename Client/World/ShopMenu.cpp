#include "ShopMenu.h"
#include "StatPanel.h"
#include "ShopOfferCard.h"
#include "SlotContainer.h"
#include "InputPromptButton.h"

#include "World/Button.h"
#include "World/Textblock.h"
#include "World/Image.h"

#include "DataManager.h"
#include "MainWorld.h"

#include "Player.h"
#include "Component/InventoryComponent.h"

#include "ItemData.h"
#include "WeaponData.h"

CShopMenu::CShopMenu()
{
}

CShopMenu::CShopMenu(const CShopMenu& src)
    : CPlayerWidgetContainer(src)
{
}

CShopMenu::~CShopMenu()
{
}

bool CShopMenu::Init()
{
    if (!CPlayerWidgetContainer::Init()) return false;

    mBackgroundImage = CreateWidget<CImage>("BackgroundImage");
    mStatPanel = CreateWidget<CStatPanel>("StatPanel");
    mItemSlotContainer = CreateWidget<CSlotContainer>("ItemSlotContainer");
    mWeaponSlotContainer = CreateWidget<CSlotContainer>("WeaponSlotContainer");
    mRerollButton = CreateWidget<CInputPromptButton>("RerollButton");
    mCurrentWaveText = CreateWidget<CTextBlock>("CurrentWaveText");
    mMaterialText = CreateWidget<CTextBlock>("MaterialText");

    auto BackgroundImage = mBackgroundImage.lock();
    if (BackgroundImage)
    {
        BackgroundImage->SetPos(0.f, 0.f);
        BackgroundImage->SetSize(1280.f, 720.f);
        BackgroundImage->SetTexture("Shop_BackgroundImage", TEXT("Custom/Shop/Background.png"));
    }

    auto StatPanel = mStatPanel.lock();
    if (StatPanel)
    {
        StatPanel->SetPos(983.f, 54.f);
        StatPanel->SetSize(260.f, 567.f);
    }

    for (int i = 0; i < 4; ++i)
    {
        mOfferCards[i] = CreateWidget<CShopOfferCard>("ShopOfferCard", 0);

        auto OfferCard = mOfferCards[i].lock();
        if (OfferCard)
        {
            OfferCard->SetPos(13.f + 241.f * i, 100.f);
            OfferCard->SetSize(235.f, 393.f);
            OfferCard->SetBanButtonClickedFunction(this, &CShopMenu::OnBanButtonClicked);
        }
    }

    CreateSlotContainer();

    CreateNextWaveButton(2);

    auto RerollButton = mRerollButton.lock();
    if(RerollButton)
    {
        RerollButton->SetPos(752.f, 28.f);
        RerollButton->SetSize(219.f, 44.f);
        RerollButton->SetLabelText(TEXT("초기화 -10"));
        RerollButton->SetButtonTint(FVector4(0.25f, 0.25f, 0.25f, 1.f));
        
        RerollButton->SetKeyIconPos(15.f, 8.f);
        RerollButton->SetKeyIconSize(28.f, 28.f);
        RerollButton->SetKeyIconTexture("Key_F", TEXT("Custom/Shop/Key_F.png"));

        RerollButton->SetTrailingIconPos(179.f, 8.f);
        RerollButton->SetTrailingIconSize(28.f, 28.f);
        RerollButton->SetTrailingIconTexture("MaterialIcon", TEXT("Custom/Material/Harvesting_Icon.png"));
        RerollButton->SetTrailingIconTint(FVector4(0.462745f, 1.f, 0.462745f, 1.f));
        RerollButton->SetClickedFunction(this, &CShopMenu::OnClickedRerollButton);
    }

    auto CurrentWaveText = mCurrentWaveText.lock();
    if (CurrentWaveText)
    {
        CurrentWaveText->SetPos(12.f, 19.f);
        CurrentWaveText->SetSize(255.f, 45.f);
        CurrentWaveText->SetFontSize(36.f);
        CurrentWaveText->SetTextColor(FVector4::White);
        CurrentWaveText->SetAlignV(ETextAlignV::Middle);
    }

    auto MaterialText = mMaterialText.lock();
    auto MaterialIcon = CreateWidget<CImage>("MaterialIcon").lock();
    if (MaterialIcon && MaterialText)
    {
        MaterialIcon->SetPos(502.f, 20.f);
        MaterialIcon->SetSize(40.f, 40.f);
        MaterialIcon->SetTexture("MaterialIcon", TEXT("Custom/Material/Harvesting_Icon.png"));
        MaterialIcon->SetTint(FVector4(0.462745f, 1.f, 0.462745f, 1.f));
        
        MaterialText->SetPos(550.f, 15.f);
        MaterialText->SetSize(190.f, 50.f);
        MaterialText->SetText(TEXT("0"));
        MaterialText->SetAlignH(ETextAlignH::Left);
        MaterialText->SetAlignV(ETextAlignV::Middle);
        MaterialText->SetTextColor(FVector4::White);
        MaterialText->SetFontSize(44.f);
    }

    return true;
}

CShopMenu* CShopMenu::Clone()
{
    return new CShopMenu(*this);
}

void CShopMenu::SubscribeDelegates(std::weak_ptr<class CPlayer> InPlayer)
{
    auto Player = InPlayer.lock();
    if (!Player) return;

    auto InvComp = Player->GetInventoryComponent().lock();
    if (!InvComp) return;

    mPlayerInvComp = InvComp;
    
    InvComp->SetUpdatedAssetsFunction(this, &CShopMenu::UpdateSlotContainer);
    InvComp->SetOnAddedMaterialFunction(this, &CShopMenu::SetMaterialText);

    auto StatPanel = mStatPanel.lock();
    if (StatPanel)
    {
        InvComp->SetOnStatUpdatedFunction(StatPanel.get(), &CStatPanel::UpdateStats);
    }
}

void CShopMenu::UnsubscribeDelegates()
{
}

void CShopMenu::CreateNextWaveButton(int NextWave)
{
    mNextWaveButton = CreateWidget<CButton>("NextWaveButton", 0);
    mNextWaveText = CreateWidget<CTextBlock>("NextWaveText", 0).lock();

    auto NextWaveButton = mNextWaveButton.lock();
    if (NextWaveButton)
    {
        for (int i = 0; i < EWidgetState::End; ++i)
        {
            NextWaveButton->SetTexture((EWidgetState::Type)i, "PauseMenuButton", TEXT("Custom/PauseMenu/Button1.png"));
        }
        NextWaveButton->SetPos(984.f, 660.f);
        NextWaveButton->SetSize(288.f, 51.f);
        NextWaveButton->SetWidgetEventFunc(EWidgetEventState::Clicked, this, &CShopMenu::OnClickedNextWaveButton);
    }

    auto NextWaveText = mNextWaveText.lock();
    if (NextWaveText)
    {
        std::wstring Text = L"Next (Wave" + std::to_wstring(NextWave) + L")";

        NextWaveButton->SetChild(NextWaveText);
        NextWaveText->SetSize(288.f, 51.f);
        NextWaveText->SetText(Text.c_str());
        NextWaveText->SetTextColor(FVector4::White);

        NextWaveText->SetAlignH(ETextAlignH::Center);
        NextWaveText->SetAlignV(ETextAlignV::Middle);
    }
}

void CShopMenu::OnClickedNextWaveButton()
{
    if(mNextWaveButtonClickedFunc)
        mNextWaveButtonClickedFunc();
}

void CShopMenu::OnBanButtonClicked(std::weak_ptr<CShopOfferCard> wOfferCard)
{
    auto OfferCard = wOfferCard.lock();
    if (OfferCard)
    {
        mBanIDs.insert(OfferCard->GetCardDesc().ID);
        OfferCard->SetEnable(false);
    }

    if (mBanIDs.size() >= 6)
    {
        for (int i = 0; i < 4; i++)
        {
            auto OfferCard = mOfferCards[i].lock();
            if (OfferCard) OfferCard->SetEnableBanButton(false);
        }

        return;
    }

    for (int i = 0; i < 4; i++)
    {
        auto OfferCard = mOfferCards[i].lock();
        if (!OfferCard) continue;

        if (OfferCard && mBanIDs.contains(OfferCard->GetCardDesc().ID))
        {
            OfferCard->SetEnable(false);
        }
    }
}

void CShopMenu::CreateSlotContainer()
{
    auto ItemSlotContainer = mItemSlotContainer.lock();
    auto WeaponSlotContainer = mWeaponSlotContainer.lock();
    if (ItemSlotContainer && WeaponSlotContainer)
    {
        ItemSlotContainer->SetPos(20.f, 570.f);
        ItemSlotContainer->SetSize(670.f, 130.f);
        ItemSlotContainer->SetStackMode(ESlotStackMode::Stack);

        WeaponSlotContainer->SetPos(748, 570.f);
        WeaponSlotContainer->SetSize(235, 130.f);
        WeaponSlotContainer->SetStackMode(ESlotStackMode::Separate);

        auto ItemText = CreateWidget<CTextBlock>("ItemTextBlock", 0).lock();
        auto WeaponText = CreateWidget<CTextBlock>("WeaponTextBlock", 0).lock();
        auto ItemHSeparator = CreateWidget<CImage>("ItemHSeparator", 0).lock();
        auto WeaponHSeperator = CreateWidget<CImage>("WeaponHSeperator", 0).lock();

        if (ItemText && WeaponText)
        {
            ItemText->SetPos(20.f, 518.f);
            ItemText->SetSize(72.f, 34.f);
            ItemText->SetFontSize(28.f);
            ItemText->SetText(TEXT("Item"));
            ItemText->SetTextColor(FVector4::White);
            ItemText->SetAlignH(ETextAlignH::Left);

            WeaponText->SetPos(748.f, 518.f);
            WeaponText->SetSize(120.f, 34.f);
            WeaponText->SetFontSize(28.f);
            WeaponText->SetText(TEXT("Weapon"));
            WeaponText->SetTextColor(FVector4::White);
            WeaponText->SetAlignH(ETextAlignH::Left);
        }

        if (ItemHSeparator && WeaponHSeperator)
        {
            ItemHSeparator->SetPos(92.f, 534.f);
            ItemHSeparator->SetSize(598.f, 2.f);
            ItemHSeparator->SetTint(FVector4(1.f, 1.f, 1.f, 0.2f));

            WeaponHSeperator->SetPos(868.f, 534.f);
            WeaponHSeperator->SetSize(115.f, 2.f);
            WeaponHSeperator->SetTint(FVector4(1.f, 1.f, 1.f, 0.2f));
        }
    }
}

void CShopMenu::CreateOfferCards()
{
    for (int i = 0; i < 4; ++i)
    {
        auto OfferCard = mOfferCards[i].lock();
        std::shared_ptr<CShopData> ShopData;
        while (true)
        {
            ShopData = CDataManager::GetInst()->GetRandomShopData();
            if (!mBanIDs.contains(ShopData->ID)) break;
        }

        if (ShopData && OfferCard)
        {
            int ID = OfferCard->GetCardDesc().ID;
            if (OfferCard->IsLocked() && !mBanIDs.contains(ID)) continue;

            FShopOfferCardDesc OfferCardDesc;
            OfferCardDesc.ID = ShopData->ID;
            OfferCardDesc.IsWeapon = ShopData->AssetType == EDataAssetType::Weapon;
            OfferCardDesc.Name = ShopData->Name;
            OfferCardDesc.IconPath = ShopData->TextureInfo.IconPath.c_str();
            OfferCardDesc.BasePrice = ShopData->BasePrice;

            bool HideBanButton = false;
            if (mBanIDs.size() >= 6) HideBanButton = true;

            OfferCard->SetCardDesc(OfferCardDesc, HideBanButton);
            OfferCard->SetEnable(true);
        }
    }
}

void CShopMenu::UpdateSlotContainer()
{
    auto ItemSlotContainer = mItemSlotContainer.lock();
    auto WeaponSlotContainer = mWeaponSlotContainer.lock();
    if (!ItemSlotContainer || !WeaponSlotContainer) return;

    // Player는 MainWorld를 통해서 얻을 수 있음 -> 결국 Update는 MainWorld에서 해줘야하나?
    // 1. MainWorld에서 Player's InventoryComponent의 OnUpdatedAssetsFunc을 구독해 ShopMenu, PauseMenu에게 전달해주는 방식
    // 2. ShopMenu, PauseMenu에서 생성될 때, 월드로부터 Player Inventory를 넘겨받아 OnUpdatedAssetsFunc를 구독해 정보를 전달받는 방식
    // 3. SlotContainer에서 MainWorld -> GetPlayerInventoryComponent() 를 통해 알아서 구독하는 방식
    //  3.1. 단, 여기서 Item / Weapon의 구분은 둬야함.
    //  3.2. 사실 Weapon이나 아니냐의 차이라서. WeaponSlotConatiner와 그 외 SlotContainer
    //  3.3. bool mIsWeaponSlot 을 두어서 구별하게 하는 게 나을 듯.
    // 
    // 방법은 2번으로 가면 될 듯.
    // 인벤토리 전달할 때 구분은 각각의 메뉴에서 하도록

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

void CShopMenu::SetMaterialText(int Material)
{
    auto MaterialText = mMaterialText.lock();
    if (MaterialText)
    {
        MaterialText->SetText(std::to_wstring(Material).c_str());
    }
}

void CShopMenu::OnClickedRerollButton()
{
    CreateOfferCards();
}

void CShopMenu::SetWaveTexts(int Wave)
{
    auto CurrentWaveText = mCurrentWaveText.lock();
    if (CurrentWaveText)
    {
        std::wstring Text = L"Shop (Wave " + std::to_wstring(Wave) + L")";

        CurrentWaveText->SetText(Text.c_str());
    }

    auto NextWaveText = mNextWaveText.lock();
    if (NextWaveText)
    {
        std::wstring Text = L"Next (Wave" + std::to_wstring(Wave + 1) + L")";

        NextWaveText->SetText(Text.c_str());
    }
}

void CShopMenu::SetUp()
{
    auto MainWorld = std::dynamic_pointer_cast<CMainWorld>(mWorld.lock());
    if (!MainWorld) return;

    CPlayerWidgetContainer::SetUp();

    SetWaveTexts(MainWorld->GetCurrentWave());
    CreateOfferCards();
}

void CShopMenu::TearDown()
{
    CPlayerWidgetContainer::TearDown();
}

void CShopMenu::InitializeWithInventory(std::weak_ptr<class CInventoryComponent> InvCompWeak)
{
    auto InvComp = InvCompWeak.lock();
    if (InvComp)
    {
        InvComp->SetUpdatedAssetsFunction(this, &CShopMenu::UpdateSlotContainer);
        InvComp->SetOnAddedMaterialFunction(this, &CShopMenu::SetMaterialText);

        mPlayerInvComp = InvComp;
    }
}

bool CShopMenu::TryPurchase(const int ID, const bool IsWeapon, int Price)
{
    // 일단 재화 없음. 그냥 구매로 감.
    auto InvComp = mPlayerInvComp.lock();
    if (InvComp)
    {
        int Materials = InvComp->GetMaterials();

        if (Materials >= Price)
        {
            if (IsWeapon)
            {
                if (InvComp->GetWeaponData().size() < 6)
                {
                    auto Data = CDataManager::GetInst()->GetWeaponData(ID);
                    InvComp->AddWeapon(Data);
                }
                else return false;
            }
            else
            {
                auto Data = CDataManager::GetInst()->GetItemData(ID);
                InvComp->AddItem(Data);
            }

            InvComp->AddMaterial(-Price);
            return true;
        }
    }

    return false;
}
