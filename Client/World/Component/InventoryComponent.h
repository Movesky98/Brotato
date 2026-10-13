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

	int mLevel = 1;
	int mRequiredXP = 40;
	int mCurrentXP = 0;

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
	std::vector<std::function<void(FStat)>> mOnStatUpdatedFuncs;
	std::vector<std::function<void(int /* Level */)>> mOnUpdatedLevelFuncs;
	std::vector<std::function<void(int /* CurrentXP */, int /* RequiredXP */)>> mOnUpdatedXPFuncs;

	void CallOnUpdatedAssetsFunction();
	void CallOnAddedMaterialFunction();
	void CallOnStatUpdatedFunction();
	void CallOnUpdatedLevelFunction();
	void CallOnUpdatedXPFunction();

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

	template<typename T>
	void SetOnStatUpdatedFunction(T* Object, void(T::* Func)(FStat))
	{
		std::function<void(FStat)> BindFunc = std::bind(Func, Object, std::placeholders::_1);

		mOnStatUpdatedFuncs.push_back(BindFunc);
	}

	template<typename T>
	void SetOnUpdatedLevelFunction(T* Object, void(T::* Func)(int))
	{
		std::function<void(int)> BindFunc = std::bind(Func, Object, std::placeholders::_1);

		mOnUpdatedLevelFuncs.push_back(BindFunc);
	}

	template<typename T>
	void SetOnUpdatedXPFunction(T* Object, void(T::* Func)(int, int))
	{
		std::function<void(int, int)> BindFunc = std::bind(Func, Object, std::placeholders::_1, std::placeholders::_2);

		mOnUpdatedXPFuncs.push_back(BindFunc);
	}

	FStat GetStat() const { return mStat; }
	int GetStatValue(EPrimaryStats Stat) { return mStat.Primary[(int)Stat]; }
	int GetStatValue(ESecondaryStats Stat) { return mStat.Secondary[(int)Stat]; }
	void AddStat(const FStat Stat);
	void AddStat(const EPrimaryStats Stat, int Value);
	void AddStat(const ESecondaryStats Stat, int Value);

	int GetMaterials() const { return mMaterial; }
	void AddMaterial(int Amount);

	int GetLevel() const { return mLevel; }
	void LevelUp();
};

