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

public:
	void SetPriceText(const TCHAR* Text);

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

