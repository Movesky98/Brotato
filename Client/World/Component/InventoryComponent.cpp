#include "InventoryComponent.h"

#include "../ItemData.h"

CInventoryComponent::CInventoryComponent()
{
}

CInventoryComponent::CInventoryComponent(const CInventoryComponent& src)
    : CActorComponent(src)
{
}

CInventoryComponent::CInventoryComponent(CInventoryComponent&& src) noexcept
    : CActorComponent(std::move(src))
{
}

CInventoryComponent::~CInventoryComponent()
{
}

bool CInventoryComponent::Init()
{
    if (!CActorComponent::Init()) return false;

    mStat.Primary[(int)EPrimaryStats::MaxHP] = 10;

    return true;
}

void CInventoryComponent::Destroy()
{


    CActorComponent::Destroy();
}

void CInventoryComponent::CallOnUpdatedAssetsFunction()
{
    for (auto It : mOnUpdatedAssetsFuncs)
    {
        if (It)
        {
            It();
        }
    }
}

void CInventoryComponent::CallOnAddedMaterialFunction()
{
    for (auto It : mOnAddedMaterialFuncs)
    {
        if (It)
        {
            It(mMaterial);
        }
    }
}

void CInventoryComponent::CallOnStatUpdatedFunction()
{
    for (auto It : mOnStatUpdatedFuncs)
    {
        if (It)
        {
            It(mStat);
        }
    }
}

void CInventoryComponent::CallOnUpdatedLevelFunction()
{
    for (auto It : mOnUpdatedLevelFuncs)
    {
        if (It)
        {
            It(mLevel);
        }
    }
}

void CInventoryComponent::CallOnUpdatedXPFunction()
{
    for (auto It : mOnUpdatedXPFuncs)
    {
        if (It)
        {
            It(mCurrentXP, mRequiredXP);
        }
    }
}

std::vector<std::weak_ptr<CDataAsset>> CInventoryComponent::GetWeaponData() const
{
    std::vector<std::weak_ptr<CDataAsset>> AssetArr;

    auto Range = mDataAssets.equal_range(EDataAssetType::Weapon);

    for (auto iter = Range.first; iter != Range.second; ++iter)
    {
        AssetArr.push_back(iter->second);
    }

    return AssetArr;
}

std::vector<std::weak_ptr<CDataAsset>> CInventoryComponent::GetItemData() const
{
    std::vector<std::weak_ptr<CDataAsset>> AssetArr;

    auto Range = mDataAssets.equal_range(EDataAssetType::Item);

    for (auto iter = Range.first; iter != Range.second; ++iter)
    {
        AssetArr.push_back(iter->second);
    }

    return AssetArr;
}

void CInventoryComponent::AddWeapon(std::weak_ptr<CDataAsset> Asset)
{
    if (Asset.expired()) return;
    if (mDataAssets.count(EDataAssetType::Weapon) >= 6) return;

    mDataAssets.insert(std::pair(EDataAssetType::Weapon, Asset));
    
    CallOnUpdatedAssetsFunction();
}

void CInventoryComponent::AddItem(std::weak_ptr<CItemData> _Asset)
{
    if (_Asset.expired()) return;

    auto Asset = _Asset.lock();
    AddStat(Asset->Stats);

    mDataAssets.insert(std::pair(EDataAssetType::Item, _Asset));

    CallOnUpdatedAssetsFunction();
}

void CInventoryComponent::AddStat(const FStat Stat)
{
    mStat += Stat; 
    
    CallOnStatUpdatedFunction();
}

void CInventoryComponent::AddStat(const EPrimaryStats Stat, int Value)
{
    mStat.Primary[(int)Stat] += Value;

    CallOnStatUpdatedFunction();
}

void CInventoryComponent::AddStat(const ESecondaryStats Stat, int Value)
{
    mStat.Secondary[(int)Stat] += Value;

    CallOnStatUpdatedFunction();
}

void CInventoryComponent::AddMaterial(int Amount)
{
    mMaterial += Amount;

    if(Amount > 0)
    {
        mCurrentXP += Amount;
        if (mCurrentXP >= mRequiredXP)
        {
            mCurrentXP = mCurrentXP - mRequiredXP;
            LevelUp();
        }

        CallOnUpdatedXPFunction();
    }

    CallOnAddedMaterialFunction();
}

void CInventoryComponent::LevelUp()
{
    mLevel++;

    mRequiredXP *= 1.2f;

    AddStat(EPrimaryStats::MaxHP, 1);

    CallOnUpdatedLevelFunction();
}
