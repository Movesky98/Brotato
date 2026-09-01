#pragma once
#include "World/WidgetContainer.h"
#include "World/Button.h"

class CInputPromptButton :
    public CWidgetContainer
{
public:
	CInputPromptButton();
	CInputPromptButton(const CInputPromptButton& src);
	virtual ~CInputPromptButton();

public:
	virtual bool Init();
	virtual CInputPromptButton* Clone();
	virtual void SetSize(float x, float y);

protected:
	std::weak_ptr<class CButton> mButton;
	std::weak_ptr<class CImage> mKeyIcon;
	std::weak_ptr<class CImage> mTrailingIcon;
	std::weak_ptr<class CTextBlock> mLabelText;

public:
	void SetKeyIconPos(float x, float y);
	void SetKeyIconSize(float x, float y);
	void SetKeyIconTexture(const std::string Name, const TCHAR* FilePath);
	void SetKeyIconEnable(bool IsEnable);

	void SetTrailingIconPos(float x, float y);
	void SetTrailingIconSize(float x, float y);
	void SetTrailingIconTexture(const std::string Name, const TCHAR* FilePath);
	void SetTrailingIconTint(FVector4 Tint);
	void SetTrailingIconEnable(bool IsEnable);
	
	void SetLabelText(const TCHAR* Text);

	template<typename T>
	void SetClickedButtonFunction(EWidgetEventState::Type Type, T* Object, void(T::* Func)())
	{
		auto Button = mButton.lock();
		if (Button)
		{
			Button->SetWidgetEventFunc(Type, Object, Func);
		}
	}
};

