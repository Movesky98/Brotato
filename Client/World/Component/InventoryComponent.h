#pragma once
#include "World/ActorComponent.h"
#include "../DataAsset.h"
#include "../GameTypes.h"

class CInventoryComponent :
    public CActorComponent
{
public:
	CInventoryComponent();
	CInventoryComponent(const CInventoryComponent& src);
	CInventoryComponent(CInventoryComponent&& src) noexcept;
	virtual ~CInventoryComponent();

protected:
	std::unordered_multimap<EDataAssetType /* AssetType */, std::weak_ptr<CDataAsset> /* DataAsset */> mDataAssets;
	FStat mStat;

public:
	virtual bool Init();
	virtual void Destroy();

public:
	virtual CInventoryComponent* Clone() const
	{
		return new CInventoryComponent(*this);
	}

protected:
	std::vector<std::function<void()>> mOnUpdatedAssetsFuncs;

	void CallOnUpdatedAssetsFunction();

public:
	std::vector<std::weak_ptr<CDataAsset>> GetWeaponData() const;
	std::vector<std::weak_ptr<CDataAsset>> GetItemData() const;

	void AddWeapon(std::weak_ptr<CDataAsset> Asset);
	void AddItem(std::weak_ptr<class CItemData> Asset);

	template<typename T>
	void SetUpdatedAssetsFunction(T* Object, void(T::* Func)())
	{
		std::function<void()> BindFunc = std::bind(Func, Object);

		mOnUpdatedAssetsFuncs.push_back(BindFunc);
	}

	const FStat GetStat() const { return mStat; }

};

