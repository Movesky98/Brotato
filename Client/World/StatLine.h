#pragma once
#include "World/WidgetContainer.h"

struct FStatLineDesc
{
    int            Value;
    const wchar_t* Name;
    const wchar_t* IconPath;
};

class CStatLine :
    public CWidgetContainer
{
public:
    CStatLine();
    CStatLine(const CStatLine& src);
    virtual ~CStatLine();

public:
    virtual bool Init();

public:
    virtual CStatLine* Clone();

protected:
    std::weak_ptr<class CImage> mIconImage;
    std::weak_ptr<class CTextBlock> mValueText;
    std::weak_ptr<class CTextBlock> mNameText;

public:
    void Initialize(const FStatLineDesc Desc);

};

