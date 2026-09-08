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

	bool CollisionMouse(std::weak_ptr<CWidget>& Result, const FVector2& MousePos) override;
	void MouseHovered() override;
	void MouseUnHovered() override;
	void MouseClicked() override;

	// 언호버 상태에서 되돌아갈 기본 색. 호버 시에는 이 색의 알파만 유지한 흰색을 쓴다.
	FVector4 mBaseTint = FVector4::White;

	void ApplyButtonTint(const FVector4& Tint);

	std::function<void()> mOnHovered;
	std::function<void()> mOnUnHovered;
	std::function<void()> mOnClicked;

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
	void SetLabelTextPos(float x, float y);
	void SetLabelTextSize(float x, float y);
	void SetLabelTextFontSize(float Size);
	void SetLabelTextColor(const FVector4 Tint);

	void SetButtonTexture(std::string Name, const TCHAR* Path);
	void SetButtonTint(const FVector4& Tint);
	void SetButtonTint(const FVector4& Tint, EWidgetState::Type Type);

	template<typename T>
	void SetHoveredFunction(T* Object, void(T::* Func)())
	{
		mOnHovered = std::bind(Func, Object);
	}

	template<typename T>
	void SetUnHoveredFunction(T* Object, void(T::* Func)())
	{
		mOnUnHovered = std::bind(Func, Object);
	}

	template<typename T>
	void SetClickedFunction(T* Object, void(T::* Func)())
	{
		mOnClicked = std::bind(Func, Object);
	}
};

