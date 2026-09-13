#pragma once

#include "Macro.h"
#include "CSVData.h"
#include "EnemyTypes.h"
#include "DataAsset.h"

class CEnemyData;
class CItemData;
class CSkillData;
class CDataAsset;
class CItemData;
class CWeaponData;
class CShopData;
class CCharacterData;
class CBossData;
class CWaveData;

class CDataManager
{
	Singleton(CDataManager);

protected:
	// 내 생각에는 unordered_map<EAssetDataType, unordered_map<ID, std::shared_ptr<CAssetData>>> 형식으로 두는 건 어떨까 싶음.
	// 그럼 템플릿 함수를 사용할 수 있게 되고 중복된 함수가 줄거든

	// -> 그냥 다 필요없고 ID는 DataAsset에 넣어라..
	std::unordered_map<std::wstring /* Skill_ID */, std::shared_ptr<CSkillData>> mSkills;
	std::unordered_map<int /* ID */, std::shared_ptr<CEnemyData>> mEnemies;
	std::unordered_map<int /* ID */, std::shared_ptr<CItemData>> mItems;
	std::unordered_map<int /* ID */, std::shared_ptr<CWeaponData>> mWeapons;
	std::unordered_map<int /* ID */, std::shared_ptr<CCharacterData>> mCharacters;
	std::unordered_map<int /* ID */, std::shared_ptr<CBossData>> mBosses;
	std::unordered_map<int /* Wave */, std::shared_ptr<CWaveData>> mWaves;

	void LoadEnemyCSV();
	void LoadSkillCSV();
	void LoadItemCSV();
	void LoadWeaponCSV();
	void LoadCharacterCSV();
	void LoadBossCSV();
	void LoadWaveCSV();

	void AddEnemyInfo(const FCSV_EnemyInfo& EnemyInfo);
	void AddTextureInfo(const FCSV_Texture& TextureInfo);
	void AddEnemySkill(const FCSV_EnemySkill& EnemySkill);

	std::shared_ptr<CSkillData> ExtractSkillDetails(const FCSV_Skill& SkillData);

public:
	void InitializeCSVData();

	std::vector<std::shared_ptr<CDataAsset>> GetEnemiesData() const;
	std::shared_ptr<CEnemyData> GetEnemyData(const int ID) const;
	std::shared_ptr<CEnemyData> GetRandomEnemyData() const;

	std::shared_ptr<CBossData> GetBossData(const int ID) const;
	std::shared_ptr<CBossData> GetRandomBossData() const;


	std::vector<std::shared_ptr<CDataAsset>> GetAllItemData() const;
	std::shared_ptr<CItemData> GetItemData(const int ID) const;
	std::shared_ptr<CItemData> GetRandomItemData() const;

	std::shared_ptr<CWeaponData> GetWeaponData(const int ID) const;

	std::vector<std::shared_ptr<CDataAsset>> GetCharactersData() const;

	std::shared_ptr<CShopData> GetRandomShopData() const;

	std::shared_ptr<CWaveData> GetWaveData(const int Wave) const;
};