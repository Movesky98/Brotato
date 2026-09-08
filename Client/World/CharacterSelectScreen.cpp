#include "CharacterSelectScreen.h"

#include "SlotContainer.h"
#include "CharacterFrame.h"
#include "InputPromptButton.h"
#include "ItemSlot.h"

#include "World/Image.h"
#include "World/TextBlock.h"
#include "World/Button.h"

#include "MainWorld.h"
#include "World/WorldManager.h"

// 테스트용
#include "DataManager.h"
#include "DataAsset.h"

CCharacterSelectScreen::CCharacterSelectScreen()
{
}

CCharacterSelectScreen::CCharacterSelectScreen(const CCharacterSelectScreen& src)
	: CWidgetContainer(src)
{
}

CCharacterSelectScreen::~CCharacterSelectScreen()
{
}

bool CCharacterSelectScreen::Init()
{
	if (!CWidgetContainer::Init()) return false;

	mBackgroundImage = CreateWidget<CImage>("BackgroundImage");
	mBackButton = CreateWidget<CInputPromptButton>("BackButton");
	mCharacterFrame = CreateWidget<CCharacterFrame>("CharacterFrame");
	mCharacterSlotContainer = CreateWidget<CSlotContainer>("CharacterSlotContainer");

	auto BackgroundImage = mBackgroundImage.lock();
	if (BackgroundImage)
	{
		BackgroundImage->SetPos(0.f, 0.f);
		BackgroundImage->SetSize(1280.f, 720.f);
		BackgroundImage->SetTexture("CharacterSelectScreen_Background", TEXT("Custom/CharacterSelectScreen/Background.png"));
	}

	auto BackButton = mBackButton.lock();
	if (BackButton)
	{
		BackButton->SetPos(16.f, 20.f);
		BackButton->SetSize(164.f, 36.f);
		
		BackButton->SetKeyIconPos(14.f, 8.f);
		BackButton->SetKeyIconSize(20.f, 20.f);
		BackButton->SetKeyIconTexture("CharacterSelectScreen_BackButton", TEXT("Custom/CharacterSelectScreen/Arrow_Left_Border.png"));
		
		BackButton->SetLabelTextPos(52.f, 6.f);
		BackButton->SetLabelTextSize(96.f, 24.f);
		BackButton->SetLabelTextFontSize(20.f);
		BackButton->SetLabelText(TEXT("뒤로"));
		BackButton->SetButtonTexture("CharacterSelectScreen_BackButton", TEXT("Custom/CharacterSelectScreen/Button.png"));
		BackButton->SetButtonTint(FVector4(0.f, 0.f, 0.f, 0.7f));

		BackButton->SetTrailingIconEnable(false);

		BackButton->SetClickedFunction(this, &CCharacterSelectScreen::OnBackButtonClicked);
	}

	auto CharacterFrame = mCharacterFrame.lock();
	if (CharacterFrame)
	{
		CharacterFrame->SetPos(141.f, 123.f);
		CharacterFrame->SetMainFrameTint(FVector4::Black);
	}

	auto CharacterSlotContainer = mCharacterSlotContainer.lock();
	if (CharacterSlotContainer)
	{
		CharacterSlotContainer->SetPos(32.f, 472.f);
		CharacterSlotContainer->SetSize(1216.f, 208.f);
		CharacterSlotContainer->ClearSlot();
		CharacterSlotContainer->SetOnHoveredFunction(this, &CCharacterSelectScreen::OnHoveredCharacterSlot);
		CharacterSlotContainer->SetOnClickedFunction(this, &CCharacterSelectScreen::OnClickedCharacterSlot);

		auto CharacterList = CDataManager::GetInst()->GetCharactersData();
		for (auto& Character : CharacterList)
		{
			CharacterSlotContainer->AddSlotData(Character);
		}
	}

	CreateRecordsPanel();
	CreateRunSettingPanel();

	return true;
}

CCharacterSelectScreen* CCharacterSelectScreen::Clone()
{
	return new CCharacterSelectScreen(*this);
}

void CCharacterSelectScreen::CreateRecordsPanel()
{
	auto RecordRoundBox = CreateWidget<CImage>("RecordRoundBox").lock();
	if (RecordRoundBox)
	{
		RecordRoundBox->SetPos(653.f, 123.f);
		RecordRoundBox->SetSize(240.f, 336.f);
		RecordRoundBox->SetTexture("CharacterSelectScreen_RecordRoundbox", TEXT("Custom/CharacterSelectScreen/Frame_Record.png"));
		RecordRoundBox->SetTint(FVector4::Black);
	}

	auto RecordIcon = CreateWidget<CImage>("RecordIcon").lock();
	if (RecordIcon)
	{
		RecordIcon->SetPos(741.f, 190.f);
		RecordIcon->SetSize(64.f, 64.f);
		RecordIcon->SetTexture("CharacterSelectScreen_RecordIcon", TEXT("Custom/CharacterSelectScreen/Record_Icon.png"));
	}

	auto RecordText = CreateWidget<CTextBlock>("RecordText").lock();
	if (RecordText)
	{
		RecordText->SetPos(669.f, 264.f);
		RecordText->SetSize(208.f, 30.f);
		RecordText->SetText(TEXT("기록"));
		RecordText->SetAlignH(ETextAlignH::Center);
		RecordText->SetAlignV(ETextAlignV::Middle);
		RecordText->SetTextColor(FVector4::White);
		RecordText->SetFontSize(18.f);
	}

	mMaxDifficultyLabel = CreateWidget<CTextBlock>("MaxDifficultyLabel").lock();
	auto MaxDifficultyLabel = mMaxDifficultyLabel.lock();
	if (MaxDifficultyLabel)
	{
		MaxDifficultyLabel->SetPos(669.f, 303.f);
		MaxDifficultyLabel->SetSize(208.f, 24.f);
		MaxDifficultyLabel->SetText(TEXT("최대 난이도 달성"));
		MaxDifficultyLabel->SetAlignH(ETextAlignH::Center);
		MaxDifficultyLabel->SetAlignV(ETextAlignV::Middle);
		MaxDifficultyLabel->SetTextColor(FVector4(1.f, 1.f, 0.f, 1.f));
		MaxDifficultyLabel->SetFontSize(15.f);
	}

	mMaxDifficultyValue = CreateWidget<CTextBlock>("MaxDifficultyValue").lock();
	auto MaxDifficultyValue = mMaxDifficultyValue.lock();
	if (MaxDifficultyValue)
	{
		MaxDifficultyValue->SetPos(669.f, 322.f);
		MaxDifficultyValue->SetSize(208.f, 24.f);
		MaxDifficultyValue->SetText(TEXT("위험 N"));
		MaxDifficultyValue->SetAlignH(ETextAlignH::Center);
		MaxDifficultyValue->SetAlignV(ETextAlignV::Middle);
		MaxDifficultyValue->SetTextColor(FVector4::White);
		MaxDifficultyValue->SetFontSize(15.f);
	}

	mEndlessWaveLabel = CreateWidget<CTextBlock>("EndlessWaveLabel").lock();
	auto EndlessWaveLabel = mEndlessWaveLabel.lock();
	if (EndlessWaveLabel)
	{
		EndlessWaveLabel->SetPos(669.f, 352.f);
		EndlessWaveLabel->SetSize(208.f, 24.f);
		EndlessWaveLabel->SetText(TEXT("무한 최대 웨이브"));
		EndlessWaveLabel->SetAlignH(ETextAlignH::Center);
		EndlessWaveLabel->SetAlignV(ETextAlignV::Middle);
		EndlessWaveLabel->SetTextColor(FVector4(1.f, 1.f, 0.f, 1.f));
		EndlessWaveLabel->SetFontSize(15.f);
	}

	mEndlessWaveValue = CreateWidget<CTextBlock>("EndlessWaveValue").lock();
	auto EndlessWaveValue = mEndlessWaveValue.lock();
	if (EndlessWaveValue)
	{
		EndlessWaveValue->SetPos(669.f, 371.f);
		EndlessWaveValue->SetSize(208.f, 24.f);
		EndlessWaveValue->SetText(TEXT("웨이브 N"));
		EndlessWaveValue->SetAlignH(ETextAlignH::Center);
		EndlessWaveValue->SetAlignV(ETextAlignV::Middle);
		EndlessWaveValue->SetTextColor(FVector4::White);
		EndlessWaveValue->SetFontSize(15.f);
	}
}

void CCharacterSelectScreen::CreateRunSettingPanel()
{
	auto RunSettingRoundBox = CreateWidget<CImage>("RunSettingRoundBox").lock();
	mEndlessModebutton = CreateWidget<CButton>("EndlessModebutton");
	mBanItemButton = CreateWidget<CButton>("BanItemButton");
	mCoOpButton = CreateWidget<CButton>("CoOpButton");

	if (RunSettingRoundBox)
	{
		RunSettingRoundBox->SetPos(923.f, 123.f);
		RunSettingRoundBox->SetSize(216.f, 336.f);
		RunSettingRoundBox->SetTexture("CharacterSelectScreen_RunSettingRoundBox", TEXT("Custom/CharacterSelectScreen/Frame_Settings.png"));
		RunSettingRoundBox->SetTint(FVector4::Black);
	}

	auto GearIcon = CreateWidget<CImage>("GearIcon").lock();
	if (GearIcon)
	{
		GearIcon->SetPos(944.f, 143.f);
		GearIcon->SetSize(32.f, 32.f);
		GearIcon->SetTexture("CharacterSelectScreen_GearIcon", TEXT("Custom/CharacterSelectScreen/Gear_Icon.png"));
	}

	auto RunSettingText = CreateWidget<CTextBlock>("RunSettingText").lock();
	if (RunSettingText)
	{
		RunSettingText->SetPos(987.f, 143.f);
		RunSettingText->SetSize(136.f, 30.f);
		RunSettingText->SetText(TEXT("달리기 설정"));
		RunSettingText->SetAlignH(ETextAlignH::Left);
		RunSettingText->SetAlignV(ETextAlignV::Middle);
		RunSettingText->SetTextColor(FVector4::White);
		RunSettingText->SetFontSize(18.f);
	}

	auto EndlessModeLabel = CreateWidget<CTextBlock>("EndlessModeLabel").lock();
	if (EndlessModeLabel)
	{
		EndlessModeLabel->SetPos(954.f, 199.f);
		EndlessModeLabel->SetSize(96.f, 30.f);
		EndlessModeLabel->SetText(TEXT("무한 모드"));
		EndlessModeLabel->SetAlignH(ETextAlignH::Left);
		EndlessModeLabel->SetAlignV(ETextAlignV::Middle);
		EndlessModeLabel->SetTextColor(FVector4::White);
		EndlessModeLabel->SetFontSize(18.f);
	}

	auto BanItemLabel = CreateWidget<CTextBlock>("BanItemLabel").lock();
	if (BanItemLabel)
	{
		BanItemLabel->SetPos(954.f, 253.f);
		BanItemLabel->SetSize(96.f, 30.f);
		BanItemLabel->SetText(TEXT("아이템 차단"));
		BanItemLabel->SetAlignH(ETextAlignH::Left);
		BanItemLabel->SetAlignV(ETextAlignV::Middle);
		BanItemLabel->SetTextColor(FVector4::White);
		BanItemLabel->SetFontSize(18.f);
	}

	auto CoOpLabel = CreateWidget<CTextBlock>("CoOpLabel").lock();
	if (CoOpLabel)
	{
		CoOpLabel->SetPos(954.f, 307.f);
		CoOpLabel->SetSize(96.f, 30.f);
		CoOpLabel->SetText(TEXT("협동 모드"));
		CoOpLabel->SetAlignH(ETextAlignH::Left);
		CoOpLabel->SetAlignV(ETextAlignV::Middle);
		CoOpLabel->SetTextColor(FVector4::White);
		CoOpLabel->SetFontSize(18.f);
	}

	auto EndlessModeButton = mEndlessModebutton.lock();
	if (EndlessModeButton)
	{
		EndlessModeButton->SetPos(1046.f, 203.f);
		EndlessModeButton->SetSize(58.f, 25.f);

		for (int i = 0; i < EWidgetState::End; ++i)
		{
			EndlessModeButton->SetTexture((EWidgetState::Type)i, "CharacterSelectScreen_ToggleOff", TEXT("Custom/CharacterSelectScreen/Toggle_Off.png"));
		}
	}

	auto BanItemButton = mBanItemButton.lock();
	if (BanItemButton)
	{
		BanItemButton->SetPos(1046.f, 257.f);
		BanItemButton->SetSize(58.f, 25.f);

		for (int i = 0; i < EWidgetState::End; ++i)
		{
			BanItemButton->SetTexture((EWidgetState::Type)i, "CharacterSelectScreen_ToggleOff", TEXT("Custom/CharacterSelectScreen/Toggle_Off.png"));
		}
	}

	auto CoOpButton = mCoOpButton.lock();
	if (CoOpButton)
	{
		CoOpButton->SetPos(1046.f, 311.f);
		CoOpButton->SetSize(58.f, 25.f);

		for (int i = 0; i < EWidgetState::End; ++i)
		{
			CoOpButton->SetTexture((EWidgetState::Type)i, "CharacterSelectScreen_ToggleOff", TEXT("Custom/CharacterSelectScreen/Toggle_Off.png"));
		}
	}
}

void CCharacterSelectScreen::OnHoveredCharacterSlot(std::weak_ptr<class CDataAsset> DataAsset)
{
	auto Data = DataAsset.lock();
	auto CharacterFrame = mCharacterFrame.lock();

	if (Data && CharacterFrame)
	{
		CharacterFrame->SetCharacterDesc(Data);
	}
}

void CCharacterSelectScreen::OnClickedCharacterSlot(std::weak_ptr<class CDataAsset> DataAsset)
{
	if (mOnCharacterSlotClicked)
		mOnCharacterSlotClicked(DataAsset);
}

void CCharacterSelectScreen::OnBackButtonClicked()
{
	if (mOnBackButtonClicked)
		mOnBackButtonClicked();

}
