#pragma once
#include "World/WidgetContainer.h"

struct FWeaponStatLineDesc
{
    const wchar_t* Name = nullptr;  // StatTable 라벨 (콜론 포함)
    std::wstring   Value;           // 표시할 값 문자열
};

// 상점 카드의 무기 스탯 한 줄. "라벨: 값" 형태로 아이콘 없이 출력한다.
class CWeaponStatLine :
    public CWidgetContainer
{
public:
    CWeaponStatLine();
    CWeaponStatLine(const CWeaponStatLine& src);
    virtual ~CWeaponStatLine();

public:
    virtual bool Init();

public:
    virtual CWeaponStatLine* Clone();

protected:
    std::weak_ptr<class CTextBlock> mNameText;
    std::weak_ptr<class CTextBlock> mValueText;

public:
    void Initialize(const FWeaponStatLineDesc Desc);
};

