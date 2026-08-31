#pragma once
#include "ShopData.h"
#include "GameTypes.h"

class CWeaponData :
    public CShopData
{
public:
    CWeaponData()
        : Type(EWeaponType::None), BaseDamage(0.f), Cooldown(0.f), Knockback(0), Critical(0.f), Range(0.f), LifeSteal(0.f), Piercing(0)
    {
        AssetType = EDataAssetType::Weapon;
    }

    EWeaponType Type;
    float BaseDamage;
    float Cooldown;
    int Knockback;
    float Critical;
    float Range;
    float LifeSteal;
    int Piercing;
};