#include "TitleScreen.h"

#include "MainWorld.h"
#include "World/WorldManager.h"

#include "World/Image.h"
#include "World/Button.h"
#include "World/TextBlock.h"

#include "Engine.h"

CTitleScreen::CTitleScreen()
{
}

CTitleScreen::CTitleScreen(const CTitleScreen& src)
	: CWidgetContainer(src)
{
}

CTitleScreen::~CTitleScreen()
{
}

bool CTitleScreen::Init()
{
	if (!CWidgetContainer::Init()) return false;

	CreateImage();

	mContinueButton = CreateButton("Continue", TEXT("Continue"), FVector2(21.f, 355.f));
	mStartButton = CreateButton("Start", TEXT("Start"), FVector2(21.f, 415.5f), true);
	mProfileButton = CreateButton("Profile", TEXT("Profile"), FVector2(21.f, 489.5f));
	mCodexButton = CreateButton("Codex", TEXT("Codex"), FVector2(21.f, 539.5f));
	mOptionButton = CreateButton("Option", TEXT("Option"), FVector2(21.f, 589.5f));
	mQuitButton = CreateButton("Quit", TEXT("Quit"), FVector2(21.f, 639.5f), true);

	auto StartButton = mStartButton.lock();
	if (StartButton)
	{
		StartButton->SetWidgetEventFunc(EWidgetEventState::Clicked, this, &CTitleScreen::OnClickedStartButton);
	}

	auto QuitButton = mQuitButton.lock();
	if (QuitButton)
	{
		QuitButton->SetWidgetEventFunc(EWidgetEventState::Clicked, this, &CTitleScreen::OnClickedQuitButton);
	}

	return true;
}

void CTitleScreen::Update(float DeltaTime)
{
	CWidgetContainer::Update(DeltaTime);

	mProgress += DeltaTime / (mPeriod * 0.5f);

	if (mProgress > 1.f)
	{
		mProgress -= 1.f;
		float Temp = mStartPos;
		mStartPos = mGoalPos;
		mGoalPos = Temp;
	}

	auto MistBack = mMistBackImage.lock();
	auto MistFront = mMistFrontImage.lock();

	if (MistBack && MistFront)
	{
		FVector3 BackPos = MistBack->GetPos();
		FVector3 FrontPos = MistFront->GetPos();
		BackPos.x = mStartPos + (mGoalPos - mStartPos) * mProgress;
		FrontPos.x = mStartPos + (mGoalPos - mStartPos) * (1.f - mProgress);

		MistBack->SetPos(BackPos);
		MistFront->SetPos(FrontPos);
	}

}

CTitleScreen* CTitleScreen::Clone()
{
	return new CTitleScreen(*this);
}

void CTitleScreen::CreateImage()
{
	auto BackgroundImage = CreateWidget<CImage>("BackgroundImage").lock();
	mMistBackImage = CreateWidget<CImage>("MistBackImage").lock();
	auto MistMidImage = CreateWidget<CImage>("MistMidImage").lock();
	mBrotatoImage = CreateWidget<CImage>("BrotatoImage").lock();
	mMistFrontImage = CreateWidget<CImage>("MistFrontImage");
	auto LogoImage = CreateWidget<CImage>("LogoImage").lock();

	if (BackgroundImage)
	{
		BackgroundImage->SetPos(-50.f, 0.f);
		BackgroundImage->SetSize(1380.f, 720.f);
		BackgroundImage->SetTexture("Title_BackgroundImage", TEXT("Custom/TitleScreen/Background.png"));
	}

	if (LogoImage)
	{
		LogoImage->SetPos(266.f, 31.33f);
		LogoImage->SetSize(748.f, 220.f);
		LogoImage->SetTexture("Logo", TEXT("Custom/TitleScreen/Logo.png"));
	}

	if (MistMidImage)
	{
		MistMidImage->SetPos(-50.f, 0.f);
		MistMidImage->SetSize(1380.f, 720.f);
		MistMidImage->SetTexture("MistMid", TEXT("Custom/TitleScreen/Mist_Mid.png"));
	}

	auto MistBackImage = mMistBackImage.lock();
	if (MistBackImage)
	{
		MistBackImage->SetPos(-100.f, 0.f);
		MistBackImage->SetSize(1380.f, 720.f);
		MistBackImage->SetTexture("MistBack", TEXT("Custom/TitleScreen/Mist_Back.png"));
	}

	auto BrotatoImage = mBrotatoImage.lock();
	if (BrotatoImage)
	{
		BrotatoImage->SetPos(-50.f, 0.f);
		BrotatoImage->SetSize(1380.f, 720.f);
		BrotatoImage->SetTexture("Brotato", TEXT("Custom/TitleScreen/Brotato.png"));
	}

	auto MistFrontImage = mMistFrontImage.lock();
	if (MistFrontImage)
	{
		MistFrontImage->SetPos(-50.f, 0.f);
		MistFrontImage->SetSize(1380.f, 720.f);
		MistFrontImage->SetTexture("MistFront", TEXT("Custom/TitleScreen/Mist_Front.png"));
	}
}

std::weak_ptr<class CButton> CTitleScreen::CreateButton(const std::string& Name, const TCHAR* Text, const FVector2& Position, bool IsActive)
{
	std::weak_ptr<CButton> TempButton = CreateWidget<CButton>(Name + std::string("Button"), 1);
	std::weak_ptr<CTextBlock> TempTextBlock = CreateWidget<CTextBlock>(Name + std::string("TextBlock"), 1);

	auto Button = TempButton.lock();
	auto TextBlock = TempTextBlock.lock();
	if (!Button || !TextBlock) return std::weak_ptr<CButton>();

	Button->SetPos(Position.x, Position.y);
	Button->SetSize(171.f, 42.f);
	Button->SetChild(TextBlock);

	for (int i = 0; i < EWidgetState::End; ++i)
	{
		Button->SetTexture((EWidgetState::Type)i, "TitleScreenButton", TEXT("Custom/TitleScreen/Button.png"));

		Button->SetTint((EWidgetState::Type)i, FVector4(0.f, 0.f, 0.f, 1.f));
	}

	if (IsActive)
	{
		Button->SetTint(EWidgetState::Hovered, FVector4(1.f, 1.f, 1.f, 1.f));
	}

	TextBlock->SetSize(171.f, 42.f);
	TextBlock->SetText(Text);
	TextBlock->SetFontSize(32.f);
	TextBlock->SetTextColor(FVector4::White);
	TextBlock->SetAlignH(ETextAlignH::Left);
	TextBlock->SetAlignV(ETextAlignV::Middle);

	return TempButton;
}

void CTitleScreen::OnClickedStartButton()
{
	if (mOnStartButtonClicked)
		mOnStartButtonClicked();
}

void CTitleScreen::OnClickedQuitButton()
{
	PostMessage(CEngine::GetInst()->GetWindowHandle(), WM_CLOSE, 0, 0);
}
