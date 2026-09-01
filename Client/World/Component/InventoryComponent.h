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
	int mMaterial = 0;

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
	std::vector<std::function<void(int)>> mOnAddedMaterialFuncs;

	void CallOnUpdatedAssetsFunction();
	void CallOnAddedMaterialFunction();

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

	template<typename T>
	void SetOnAddedMaterialFunction(T* Object, void(T::* Func)(int))
	{
		std::function<void(int)> BindFunc = std::bind(Func, Object, std::placeholders::_1);

		mOnAddedMaterialFuncs.push_back(BindFunc);
	}

	const FStat GetStat() const { return mStat; }
	int GetMaterial() const { return mMaterial; }
	void AddMaterial(int Amount);
};

