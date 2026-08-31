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
    mStat += Asset->Stats;

    mDataAssets.insert(std::pair(EDataAssetType::Item, _Asset));

    CallOnUpdatedAssetsFunction();
}
