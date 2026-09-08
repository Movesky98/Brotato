#pragma once
#include "DataAsset.h"

class CCharacterData :
    public CDataAsset
{
public:
    CCharacterData();

    FStat Stats;

    std::weak_ptr<class CWeaponData> StartWeapon;
};

