#include "DataManager.h"

#include "CSVReader.h"
#include "DataAsset.h"

#include "EnemyData.h"
#include "SkillData.h"
#include "DashSkillData.h"
#include "SpawnProjectileSkillData.h"
#include "SpawnSlashSkillData.h"

#include "ShopData.h"
#include "ItemData.h"
#include "WeaponData.h"
#include "CharacterData.h"
#include "BossData.h"

#include <random>

CDataManager* CDataManager::mInstance = nullptr;

CDataManager::CDataManager()
{

}

CDataManager::~CDataManager()
{

}

void CDataManager::InitializeCSVData()
{
	LoadSkillCSV();
	LoadEnemyCSV();
	LoadItemCSV();
	LoadWeaponCSV();
	LoadCharacterCSV();
	LoadBossCSV();
}

void CDataManager::LoadEnemyCSV()
{
	// 기본적인 적 정보
	auto EnemyInfoList = CSVReader::ReadData<FCSV_EnemyInfo>(TEXT("CSV/EnemyInfo.csv"));
	for (const FCSV_EnemyInfo& EnemyInfo : EnemyInfoList)
	{
		AddEnemyInfo(EnemyInfo);
	}

	// 적 텍스쳐
	auto TextureDataList = CSVReader::ReadData<FCSV_Texture>(TEXT("CSV/Texture.csv"));
	for (const FCSV_Texture& TextureData : TextureDataList)
	{
		int ID = TextureData.ID;
		AddTextureInfo(TextureData);
	}

	if (!mSkills.size()) return;

	// 적 스킬
	auto EnemySkillDataList = CSVReader::ReadData<FCSV_EnemySkill>(TEXT("CSV/EnemySkill.csv"));
	for (const auto& EnemySkillData : EnemySkillDataList)
	{
		AddEnemySkill(EnemySkillData);
	}
}

void CDataManager::LoadSkillCSV()
{
	// TODO :
	// SkillInfos 초기화
	// ComponentType으로 make_shared<파생 클래스>를 이용해서 부모한테 저장
	auto SkillDataList = CSVReader::ReadData<FCSV_Skill>(TEXT("CSV/Skills.csv"));
	for (const auto& SkillData : SkillDataList)
	{
		std::shared_ptr<CSkillData> NewSkill = ExtractSkillDetails(SkillData);

		if (NewSkill)
		{
			std::wstring ID = SkillData.SkillID;
			NewSkill->Component = SkillData.Component;
			NewSkill->Target = SkillData.Target;
			NewSkill->Cooldown = SkillData.Cooldown;

			mSkills.insert(std::make_pair(ID, NewSkill));
		}
	}
}

void CDataManager::LoadItemCSV()
{
	auto ItemDataList = CSVReader::ReadData<FCSV_ItemDesc>(TEXT("CSV/Item/ItemDesc.csv"));
	for (const auto& ItemData : ItemDataList)
	{
		int ID = ItemData.ID;
		std::shared_ptr<CItemData> NewItem = std::make_shared<CItemData>();

		NewItem->ID = ID;
		NewItem->Name = ItemData.Name;
		NewItem->Stats = ItemData.Stats;
		NewItem->BasePrice = ItemData.BasePrice;
		mItems.insert(std::make_pair(ID, NewItem));
	}

	auto ItemTextureList = CSVReader::ReadData<FCSV_Texture>(TEXT("CSV/Item/ItemTexture.csv"));
	for (const auto& TextureData : ItemTextureList)
	{
		int ID = TextureData.ID;
		auto iter = mItems.find(ID);
		if (iter != mItems.end())
		{
			std::shared_ptr<CItemData> Data = iter->second;
			if (Data)
			{
				Data->TextureInfo.Path = TextureData.Path;
				Data->TextureInfo.SizeX = TextureData.SizeX;
				Data->TextureInfo.SizeY = TextureData.SizeY;
				Data->TextureInfo.IconPath = TextureData.IconPath;
				Data->TextureInfo.IconSizeX = TextureData.IconSizeX;
				Data->TextureInfo.IconSizeY = TextureData.IconSizeY;
			}
		}
	}
}

void CDataManager::LoadWeaponCSV()
{
	auto WeaponDataList = CSVReader::ReadData<FCSV_WeaponDesc>(TEXT("CSV/Weapon/WeaponDesc.csv"));
	for (const auto& WeaponData : WeaponDataList)
	{
		int ID = WeaponData.ID;
		std::shared_ptr<CWeaponData> NewWeapon = std::make_shared<CWeaponData>();

		NewWeapon->ID = ID;
		NewWeapon->Name = WeaponData.Name;
		NewWeapon->Type = WeaponData.Type;
		NewWeapon->BaseDamage = WeaponData.Damage;
		NewWeapon->Critical = WeaponData.Critical;
		NewWeapon->Cooldown = WeaponData.Cooldown;
		NewWeapon->Knockback = WeaponData.Knockback;
		NewWeapon->Range = WeaponData.Range;
		NewWeapon->Piercing = WeaponData.Piercing;
		NewWeapon->BasePrice = WeaponData.BasePrice;

		mWeapons.insert(std::make_pair(ID, NewWeapon));
	}

	auto TextureList = CSVReader::ReadData<FCSV_Texture>(TEXT("CSV/Weapon/WeaponTexture.csv"));
	for (const auto& Texture : TextureList)
	{
		int ID = Texture.ID;
		auto iter = mWeapons.find(ID);
		if (iter != mWeapons.end())
		{
			std::shared_ptr<CWeaponData> Data = iter->second;
			if (Data)
			{
				Data->TextureInfo.Path = Texture.Path;
				Data->TextureInfo.SizeX = Texture.SizeX;
				Data->TextureInfo.SizeY = Texture.SizeY;
				Data->TextureInfo.IconPath = Texture.IconPath;
				Data->TextureInfo.IconSizeX = Texture.IconSizeX;
				Data->TextureInfo.IconSizeY = Texture.IconSizeY;
			}
		}
	}
}

void CDataManager::LoadCharacterCSV()
{
	auto CharacterDataList = CSVReader::ReadData<FCSV_CharacterDesc>(TEXT("CSV/Character/CharacterDesc.csv"));
	for (const auto Data : CharacterDataList)
	{
		int ID = Data.ID;
		std::shared_ptr<CCharacterData> NewCharacter = std::make_shared<CCharacterData>();

		NewCharacter->ID = ID;
		NewCharacter->Name = Data.Name;
		NewCharacter->Stats = Data.Stats;

		auto iter = mWeapons.find(Data.StartWeaponID);
		if (iter != mWeapons.end())
		{
			NewCharacter->StartWeapon = iter->second;
		}

		mCharacters.insert(std::make_pair(ID, NewCharacter));
	}

	auto TextureList = CSVReader::ReadData<FCSV_Texture>(TEXT("CSV/Character/CharacterTexture.csv"));
	for (const auto& Texture : TextureList)
	{
		int ID = Texture.ID;
		auto iter = mCharacters.find(ID);
		if (iter != mCharacters.end())
		{
			std::shared_ptr<CCharacterData> Data = iter->second;
			if (Data)
			{
				Data->TextureInfo.Path = Texture.Path;
				Data->TextureInfo.SizeX = Texture.SizeX;
				Data->TextureInfo.SizeY = Texture.SizeY;
				Data->TextureInfo.IconPath = Texture.IconPath;
				Data->TextureInfo.IconSizeX = Texture.IconSizeX;
				Data->TextureInfo.IconSizeY = Texture.IconSizeY;
			}
		}
	}
}

void CDataManager::LoadBossCSV()
{
	auto BossDataList = CSVReader::ReadData<FCSV_BossDesc>(TEXT("CSV/Boss/BossDesc.csv"));
	for (const auto& Data : BossDataList)
	{
		int ID = Data.ID;
		std::shared_ptr<CBossData> NewBoss = std::make_shared<CBossData>();
		
		NewBoss->Name = Data.Name;
		NewBoss->HP = Data.HP;
		NewBoss->Speed = Data.Speed;
		NewBoss->Damage = Data.Damage;
		NewBoss->MovementTypes = Data.MovementTypes;

		mBosses.insert(std::make_pair(ID, NewBoss));
	}

	// 스킬 넘겨주기
	auto TextureList = CSVReader::ReadData<FCSV_Texture>(TEXT("CSV/Boss/BossTexture.csv"));
	for (const auto& Texture : TextureList)
	{
		int ID = Texture.ID;
		auto iter = mBosses.find(ID);
		if (iter != mBosses.end())
		{
			std::shared_ptr<CBossData> Data = iter->second;
			if (Data)
			{
				Data->TextureInfo.Path = Texture.Path;
				Data->TextureInfo.SizeX = Texture.SizeX;
				Data->TextureInfo.SizeY = Texture.SizeY;
				Data->TextureInfo.IconPath = Texture.IconPath;
				Data->TextureInfo.IconSizeX = Texture.IconSizeX;
				Data->TextureInfo.IconSizeY = Texture.IconSizeY;
			}
		}
	}
}

void CDataManager::AddEnemyInfo(const FCSV_EnemyInfo& EnemyInfo)
{
	int ID = EnemyInfo.ID;
	std::shared_ptr<CEnemyData> NewEnemy = std::make_shared<CEnemyData>();
	
	NewEnemy->ID = ID;
	NewEnemy->Name = EnemyInfo.Name;
	NewEnemy->HP = EnemyInfo.HP;
	NewEnemy->Speed = EnemyInfo.Speed;
	NewEnemy->Damage = EnemyInfo.Damage;
	NewEnemy->Movement = EnemyInfo.Movement;

	mEnemies.insert(std::make_pair(ID, NewEnemy));
}

void CDataManager::AddTextureInfo(const FCSV_Texture& TextureInfo)
{
	int ID = TextureInfo.ID;

	auto iter = mEnemies.find(ID);
	if (iter != mEnemies.end())
	{
		std::shared_ptr<CEnemyData> Data = iter->second;
		if (Data)
		{
			Data->TextureInfo.Path = TextureInfo.Path;
			Data->TextureInfo.SizeX = TextureInfo.SizeX;
			Data->TextureInfo.SizeY = TextureInfo.SizeY;
			Data->TextureInfo.IconPath = TextureInfo.IconPath;
			Data->TextureInfo.IconSizeX = TextureInfo.IconSizeX;
			Data->TextureInfo.IconSizeY = TextureInfo.IconSizeY;
		}
	}
}

void CDataManager::AddEnemySkill(const FCSV_EnemySkill& EnemySkill)
{
	int ID = EnemySkill.ID;

	// 적 ID를 가진 정보가 없으면 넘어감
	auto iter = mEnemies.find(ID);
	if (iter == mEnemies.end()) return;

	std::shared_ptr<CEnemyData>& CurrentEnemy = iter->second;
	// CurrentEnemy->SkillData.ID = ID;

	// 저장된 적 정보에 Skill Data를 저장함.
	for (const auto& SkillID : EnemySkill.SkillIDs)
	{
		auto Skilliter = mSkills.find(SkillID);
		if (Skilliter != mSkills.end())
		{
			CurrentEnemy->Skills.insert(std::make_pair(EnemySkill.Trigger, Skilliter->second));
		}
	}
}

std::shared_ptr<CSkillData> CDataManager::ExtractSkillDetails(const FCSV_Skill& SkillData)
{
	std::wstring ID = SkillData.SkillID;
	ESkillComponent ComponentType = SkillData.Component;

	std::shared_ptr<CSkillData> TempPtr;

	switch (ComponentType)
	{
	case ESkillComponent::Dash:
	{
		auto DashSkillDataList = CSVReader::ReadData<FCSV_DashSkill>(TEXT("CSV/Skill_Dash.csv"));

		for (const auto& DashSkillData : DashSkillDataList)
		{
			if (DashSkillData.SkillID == ID)
			{
				TempPtr = std::make_shared<CDashSkillData>();

				auto DashSkillPtr = std::dynamic_pointer_cast<CDashSkillData>(TempPtr);

				DashSkillPtr->Distance = DashSkillData.Distance;
				DashSkillPtr->Speed = DashSkillData.Speed;
				break;
			}
		}
	}
		break;

	case ESkillComponent::SpawnProjectile:
	{
		auto SpawnProjectileDataList = CSVReader::ReadData<FCSV_SpawnProjectileSkill>(TEXT("CSV/Skill_SpawnProjectile.csv"));
		for (const auto& SPData : SpawnProjectileDataList)
		{
			if (SPData.SkillID == ID)
			{
				TempPtr = std::make_shared<CSpawnProjectileSkillData>();

				auto SPSkillPtr = std::dynamic_pointer_cast<CSpawnProjectileSkillData>(TempPtr);

				SPSkillPtr->Count = SPData.Count;
				SPSkillPtr->Interval = SPData.Interval;
				SPSkillPtr->Spread = SPData.Spread;
				break;
			}
		}
	}
		break;
	case ESkillComponent::SpawnSlash:
	{
		auto SpawnSlashDataList = CSVReader::ReadData<FCSV_SpawnSlashSkill>(TEXT("CSV/Skill_SpawnSlash.csv"));
		for (const auto& Data : SpawnSlashDataList)
		{
			if (Data.SkillID == ID)
			{
				TempPtr = std::make_shared<CSpawnSlashSkillData>();

				auto SkillPtr = std::dynamic_pointer_cast<CSpawnSlashSkillData>(TempPtr);

				SkillPtr->Count = Data.Count;
				SkillPtr->Interval = Data.Interval;
				SkillPtr->Spread = Data.Spread;
				SkillPtr->MoveType = Data.MoveType;
				break;
			}
		}
	}
		break;
	default:
		break;
	}

	return TempPtr;
}

std::vector<std::shared_ptr<CDataAsset>> CDataManager::GetEnemiesData() const
{
	std::vector<std::shared_ptr<CDataAsset>> EnemiesArr;

	for (auto& [ID, Enemy] : mEnemies)
	{
		if (Enemy)
		{
			EnemiesArr.push_back(Enemy);
		}
	}

	return EnemiesArr;
}

std::shared_ptr<CEnemyData> CDataManager::GetEnemyData(const int ID) const
{
	auto iter = mEnemies.find(ID);
	if (iter != mEnemies.end())
	{
		return iter->second;
	}

	return std::shared_ptr<CEnemyData>();
}

std::shared_ptr<CEnemyData> CDataManager::GetRandomEnemyData() const
{
	if (!mEnemies.size()) return std::shared_ptr<CEnemyData>();

	int ID = rand() % mEnemies.size();
	ID += 1000;		  
	
	return GetEnemyData(ID);
}

std::shared_ptr<CBossData> CDataManager::GetBossData(const int ID) const
{
	auto iter = mBosses.find(ID);
	if (iter != mBosses.end())
	{
		return iter->second;
	}

	return std::shared_ptr<CBossData>();
}

std::shared_ptr<CBossData> CDataManager::GetRandomBossData() const
{
	if (!mBosses.size()) return std::shared_ptr<CBossData>();

	int ID = rand() % mBosses.size();
	ID += 4000;

	return GetBossData(ID);
}

std::vector<std::shared_ptr<CDataAsset>> CDataManager::GetAllItemData() const
{
	if(!mItems.size()) return std::vector<std::shared_ptr<CDataAsset>>();

	std::vector<std::shared_ptr<CDataAsset>> ItemsArr;

	for (auto& [ID, Item] : mItems)
	{
		if (Item)
		{
			ItemsArr.push_back(Item);
		}
	}

	return ItemsArr;
}

std::shared_ptr<CItemData> CDataManager::GetItemData(const int ID) const
{
	auto iter = mItems.find(ID);

	if (iter != mItems.end())
	{
		return iter->second;
	}

	return std::shared_ptr<CItemData>();
}

std::shared_ptr<CItemData> CDataManager::GetRandomItemData() const
{
	if (!mItems.size()) return std::shared_ptr<CItemData>();

	int ID = rand() % mItems.size();
	ID += 2000;

	return GetItemData(ID);
}

std::shared_ptr<CWeaponData> CDataManager::GetWeaponData(const int ID) const
{
	auto iter = mWeapons.find(ID);

	if (iter != mWeapons.end())
	{
		return iter->second;
	}

	return std::shared_ptr<CWeaponData>();
}

std::vector<std::shared_ptr<CDataAsset>> CDataManager::GetCharactersData() const
{
	if (!mCharacters.size()) return std::vector<std::shared_ptr<CDataAsset>>();

	std::vector<std::shared_ptr<CDataAsset>> CharacterArr;

	for (auto& [ID, Character] : mCharacters)
	{
		if (Character)
		{
			CharacterArr.push_back(Character);
		}
	}

	return CharacterArr;
}

std::shared_ptr<CShopData> CDataManager::GetRandomShopData() const
{
	if (!mItems.size() || !mWeapons.size()) return std::shared_ptr<CShopData>();

	int Type = rand() % 2 ? 2000 : 3000;
	int Rand = 0;

	if (Type == 2000) 
	{

		Rand = rand() % mItems.size();
		return GetItemData(Rand + Type);
	}
	else
	{
		Rand = rand() % mWeapons.size();
		
		return GetWeaponData(Rand + Type);
	}
}
