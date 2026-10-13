#pragma once
#include "World/WidgetContainer.h"

class CStatRow :
    public CWidgetContainer
{
public:
    CStatRow();
    CStatRow(const CStatRow& src);
    virtual ~CStatRow();

public:
    virtual bool Init();

public:
    virtual CStatRow* Clone();

protected:
    std::weak_ptr<class CImage> mIconImage;
    std::weak_ptr<class CTextBlock> mNameText;
    std::weak_ptr<class CTextBlock> mValueText;

    void SetColor(const FVector4 Color);

public:
    void SetSize(float x, float y) override;
 
    void SetIconVisible(bool IsVisible);
    void SetIconTexture(const std::string Name, const TCHAR* Path);
    void SetImage(const TCHAR* Path);
    void SetNameText(const TCHAR* Text);
    void SetValueText(int Value, bool UseColor = true);
};

