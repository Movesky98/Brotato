#pragma once

#include "EngineInfo.h"
#include "EnemyTypes.h"
#include "GameTypes.h"
#include "ConvertString.h"

#include <sstream>

bool ConvertWideToUTF8(const std::wstring& In, std::string& Out);
bool ConvertWideToMovement(const std::wstring& In, std::vector<EEnemyMovement>& Out);
bool ConvertWideToMovement(const std::wstring& In, std::vector<FMovementType>& Out);
bool ConvertWideToTrigger(const std::wstring& In, ESkillTrigger& Out);
bool SplitDataToSkills(const std::wstring& In, std::vector<std::wstring>& Out);
bool ConvertWideToSkillComponent(const std::wstring& In, ESkillComponent& Out);
bool ConvertWideToSkillTarget(const std::wstring& In, ESkillTarget& Out);

/// <summary>
/// <para>Stat은 "속성:값|속성:값..." 형태로 들어옴.</para>
/// <para>이 함수에서 '|'를 기준으로 토큰 분리를 하고</para>
/// <para>이후 분리된 토큰을 가지고 속성 값과 속성이 지니는 값을 구분하여 Out에 저장함.</para>
/// </summary>
bool ConvertWideToPrimaryStats(const std::wstring& In, FStat& Out);
bool ConvertWideToSecondaryStats(const std::wstring& In, FStat& Out);

bool ConvertWideToWeaponType(const std::wstring& In, EWeaponType& Out);

//**********************************************************************
//                                Texture
//**********************************************************************
struct FCSV_Texture
{
	int ID;
	std::wstring Path;
	float SizeX;
	float SizeY;
	std::wstring IconPath;
	float IconSizeX;
	float IconSizeY;

	bool Init(const std::vector<std::wstring>& Data)
	{
		if (Data.size() != 7)
			return false;

		ID = std::stoi(Data[0]);
		Path = Data[1];
		SizeX = std::stof(Data[2]);
		SizeY = std::stof(Data[3]);
		IconPath = Data[4];
		IconSizeX = std::stof(Data[5]);
		IconSizeY = std::stof(Data[6]);

		return true;
	}

	FCSV_Texture()
		: ID(0), Path(TEXT("")), SizeX(0.f), SizeY(0.f), IconPath(TEXT("")), IconSizeX(0.f), IconSizeY(0.f)
	{
	}
};

//**********************************************************************
//                                Enemy
//**********************************************************************
struct FCSV_EnemyInfo
{
public:
	int ID;
	std::string Name;
	float HP;
	float Speed;
	float Damage;
	std::vector<EEnemyMovement> Movement;

	bool Init(const std::vector<std::wstring>& Data)
	{
		if (Data.size() != 6)
			return false;

		ID = std::stoi(Data[0]);

		if (!ConvertWideToUTF8(Data[1], Name))
			return false;

		HP = std::stof(Data[2]);
		Speed = std::stof(Data[3]);
		Damage = std::stof(Data[4]);

		if (!ConvertWideToMovement(Data[5], Movement))
			return false;

		return true;
	}

	FCSV_EnemyInfo()
		: ID(0), Name(""), HP(0.f), Speed(0.f), Damage(0.f)
	{ }
};

struct FCSV_EnemySkill
{
public:
	int ID;
	ESkillTrigger Trigger;
	std::vector<std::wstring> SkillIDs;

	bool Init(const std::vector<std::wstring>& Data)
	{
		if (Data.size() != 3)
			return false;

		ID = std::stoi(Data[0]);

		if (!ConvertWideToTrigger(Data[1], Trigger))
			return false;

		if (!SplitDataToSkills(Data[2], SkillIDs))
			return false;

		return true;
	}

	FCSV_EnemySkill()
		: ID(0), Trigger(ESkillTrigger::None)
	{ }
};

//**********************************************************************
//                                Skill
//**********************************************************************

struct FCSV_Skill
{
	std::wstring SkillID;
	ESkillComponent Component;
	ESkillTarget Target;
	float Cooldown;

	virtual bool Init(const std::vector<std::wstring>& Data)
	{
		if (Data.size() != 4)
			return false;

		SkillID = Data[0];
		if (!ConvertWideToSkillComponent(Data[1], Component))
			return false;

		if (!ConvertWideToSkillTarget(Data[2], Target))
			return false;

		Cooldown = std::stof(Data[3]);


		return true;
	}

	FCSV_Skill()
		: Component(ESkillComponent::None), Target(ESkillTarget::None), Cooldown(0)
	{ }

	virtual ~FCSV_Skill() = default;
};

struct FCSV_SpawnProjectileSkill : public FCSV_Skill
{
	int Count;			// 발사 개수
	float Interval;		// 발사 간격(초)
	int Spread;			// 발사 각도 (얼마나 퍼질건지)

	bool Init(const std::vector<std::wstring>& Data) override
	{
		if (Data.size() != 4)
			return false;

		SkillID = Data[0];
		Count = std::stoi(Data[1]);
		Interval = std::stof(Data[2]);
		Spread = std::stoi(Data[3]);

		return true;
	}

	FCSV_SpawnProjectileSkill()
		: Count(1), Interval(0.f), Spread(0)
	{
	}
};

struct FCSV_DashSkill : public FCSV_Skill
{
	float Distance;
	float Speed;

	bool Init(const std::vector<std::wstring>& Data) override
	{
		if (Data.size() != 3)
			return false;

		SkillID = Data[0];
		Distance = std::stof(Data[1]);
		Speed = std::stof(Data[2]);

		return true;
	}

	FCSV_DashSkill()
		: Distance(100.f), Speed(300.f)
	{
	}
};

struct FCSV_SpawnSlashSkill : public FCSV_Skill
{
	int Count;				// 발사 개수
	float Interval;			// 발사 간격(초)
	int Spread;				// 발사 각도
	int MoveType;			// 움직임 타입 (Stop = 0, Move = 1)

	bool Init(const std::vector<std::wstring>& Data) override
	{
		if (Data.size() != 5)
			return false;

		SkillID = Data[0];
		Count = std::stoi(Data[1]);
		Interval = std::stof(Data[2]);
		Spread = std::stoi(Data[3]);
		MoveType = std::stoi(Data[4]);

		return true;
	}

	FCSV_SpawnSlashSkill()
		: Count(1), Interval(0.f), Spread(0), MoveType(-1)
	{
	}
};

struct FCSV_SpawnEnemySkill : public FCSV_Skill
{
	int Count;				// 소환 마리 수
	float Interval;			// 소환 간격(초)
	int EnemyID;			// 소환할 적 ID

	bool Init(const std::vector<std::wstring>& Data) override
	{
		if (Data.size() != 4)
			return false;

		SkillID = Data[0];
		Count = std::stoi(Data[1]);
		Interval = std::stof(Data[2]);
		EnemyID = std::stoi(Data[3]);

		return true;
	}

	FCSV_SpawnEnemySkill()
		: Count(1), Interval(0.f), EnemyID(0)
	{
	}
};

//**********************************************************************
//                                Item
//**********************************************************************
struct FCSV_ItemDesc
{
	int ID;
	std::string Name;
	FStat Stats;
	int BasePrice;

	bool Init(const std::vector<std::wstring>& Data)
	{
		if (Data.size() != 5) return false;

		ID = std::stoi(Data[0]);
		if (!ConvertWideToUTF8(Data[1], Name)) return false;
		ConvertWideToPrimaryStats(Data[2], Stats);
		ConvertWideToSecondaryStats(Data[3], Stats);
		BasePrice = std::stoi(Data[4]);

		return true;
	}

	FCSV_ItemDesc()
		: ID(0), Name(""), BasePrice(0)
	{ }
};

//**********************************************************************
//                                Weapon
//**********************************************************************
struct FCSV_WeaponDesc
{
	int				ID;
	std::string		Name;
	EWeaponType		Type;
	int				Damage;			// 대미지
	float			Critical;		// 치명타 (%)
	float			Cooldown;		// 쿨타임
	int				Knockback;		// 넉백
	float			Range;			// 범위
	int				Piercing;		// 관통
	int				BasePrice;		// 기본 가격

	bool Init(const std::vector<std::wstring>& Data)
	{
		if (Data.size() != 10) return false;

		ID = std::stoi(Data[0]);
		if (!ConvertWideToUTF8(Data[1], Name)) return false;
		if (!ConvertWideToWeaponType(Data[2], Type)) return false;
		Damage = std::stoi(Data[3]);
		Critical = std::stof(Data[4]);
		Cooldown = std::stof(Data[5]);
		Knockback = std::stoi(Data[6]);
		Range = std::stof(Data[7]);
		Piercing = std::stoi(Data[8]);
		BasePrice = std::stoi(Data[9]);

		return true;
	}

	FCSV_WeaponDesc()
		: ID(0), Name(""), Type(EWeaponType::None), Damage(0), Critical(0.f), Cooldown(0.f), Knockback(0), Range(0.f), Piercing(0), BasePrice(0)
	{ }
};

//**********************************************************************
//                                Character
//**********************************************************************
struct FCSV_CharacterDesc
{
	int ID;
	std::string Name;
	FStat Stats;
	int StartWeaponID;

	bool Init(const std::vector<std::wstring>& Data)
	{
		if (Data.size() != 5) return false;

		ID = std::stoi(Data[0]);
		if (!ConvertWideToUTF8(Data[1], Name)) return false;
		ConvertWideToPrimaryStats(Data[2], Stats);
		ConvertWideToSecondaryStats(Data[3], Stats);
		StartWeaponID = std::stoi(Data[4]);

		return true;
	}

	FCSV_CharacterDesc()
		: ID(0), Name(""), StartWeaponID(0)
	{

	}
};

//**********************************************************************
//                                Boss
//**********************************************************************

struct FCSV_BossDesc
{
public:
	int ID;
	std::string Name;
	float HP;
	float Speed;
	float Damage;
	std::vector<FMovementType> MovementTypes;

	bool Init(const std::vector<std::wstring>& Data)
	{
		if (Data.size() != 6)
			return false;

		ID = std::stoi(Data[0]);

		if (!ConvertWideToUTF8(Data[1], Name))
			return false;

		HP = std::stof(Data[2]);
		Speed = std::stof(Data[3]);
		Damage = std::stof(Data[4]);

		if (!ConvertWideToMovement(Data[5], MovementTypes))
			return false;

		return true;
	}

	FCSV_BossDesc()
		: ID(0), Name(""), HP(0.f), Speed(0.f), Damage(0.f)
	{
	}
};