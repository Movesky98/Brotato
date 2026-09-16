#pragma once
#include "World/WidgetContainer.h"

#include "World/Button.h"

class CBuyButton :
    public CWidgetContainer
{
public:
	CBuyButton();
	CBuyButton(const CBuyButton& src);
	virtual ~CBuyButton();

	virtual void SetSize(float x, float y);

public:
	virtual bool Init();
	virtual CBuyButton* Clone();

protected:
	std::weak_ptr<class CImage> mIcon;
	std::weak_ptr<class CTextBlock> mPriceText;
	std::weak_ptr<class CButton> mButton;

	bool CollisionMouse(std::weak_ptr<CWidget>& Result, const FVector2& MousePos) override;
	void MouseHovered() override;
	void MouseUnHovered() override;
	void MouseClicked() override;

	// 언호버 상태에서 되돌아갈 기본 색. 호버 시에는 이 색의 알파만 유지한 흰색을 쓴다.
	FVector4 mBaseTint = FVector4(0.5f, 0.5f, 0.5f, 1.f);

	void ApplyButtonTint(const FVector4& Tint);

	std::function<void()> mOnHovered;
	std::function<void()> mOnUnHovered;
	std::function<void()> mOnClicked;

public:
	void SetPriceText(const TCHAR* Text);

	void SetButtonTint(const FVector4& Tint);

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
