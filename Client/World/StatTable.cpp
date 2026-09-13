#include "StatTable.h"

namespace
{
	struct FPrimaryStatEntry
	{
		const wchar_t* Name;
		EPrimaryStats Value;
	};

	struct FSecondaryStatEntry
	{
		const wchar_t* Name;
		ESecondaryStats Value;
	};

	struct FWeaponStatEntry
	{
		const wchar_t* Name;
		EWeaponStats Value;
	};

	const FPrimaryStatEntry PrimaryStatTable[] =
	{
		{ L"MaxHP", EPrimaryStats::MaxHP },
		{ L"HPRegeneration", EPrimaryStats::HPRegeneration },
		{ L"LifeSteal", EPrimaryStats::LifeSteal },
		{ L"Damage", EPrimaryStats::Damage },
		{ L"MeleeDamage", EPrimaryStats::MeleeDamage },
		{ L"RangedDamage", EPrimaryStats::RangedDamage },
		{ L"ElementalDamage", EPrimaryStats::ElementalDamage },
		{ L"AttackSpeed", EPrimaryStats::AttackSpeed },
		{ L"CritChance", EPrimaryStats::CritChance },
		{ L"Engineering", EPrimaryStats::Engineering },
		{ L"Range", EPrimaryStats::Range },
		{ L"Armor", EPrimaryStats::Armor },
		{ L"Dodge", EPrimaryStats::Dodge },
		{ L"Speed", EPrimaryStats::Speed },
		{ L"Luck", EPrimaryStats::Luck },
		{ L"Harvesting", EPrimaryStats::Harvesting },
	};

	const FSecondaryStatEntry SecondaryStatTable[] =
	{
		{ L"ConsumableHeal", ESecondaryStats::ConsumableHeal },
		{ L"MaterialsHealing", ESecondaryStats::MaterialsHealing },
		{ L"XPGain", ESecondaryStats::XPGain },
		{ L"PickupRange", ESecondaryStats::PickupRange },
		{ L"ItemsPrice", ESecondaryStats::ItemsPrice },
		{ L"ExplosionDamage", ESecondaryStats::ExplosionDamage },
		{ L"ExplosionSize", ESecondaryStats::ExplosionSize },
		{ L"Bounces", ESecondaryStats::Bounces },
		{ L"Piercing", ESecondaryStats::Piercing },
		{ L"PiercingDamage", ESecondaryStats::PiercingDamage },
		{ L"DamageAgainstBosses", ESecondaryStats::DamageAgainstBosses },
		{ L"StructureAttackSpeed", ESecondaryStats::StructureAttackSpeed },
		{ L"StructureRange", ESecondaryStats::StructureRange },
		{ L"BurningSpeed", ESecondaryStats::BurningSpeed },
		{ L"BurningSpread", ESecondaryStats::BurningSpread },
		{ L"Knockback", ESecondaryStats::Knockback },
		{ L"DoubleMaterialChance", ESecondaryStats::DoubleMaterialChance },
		{ L"FreeRerolls", ESecondaryStats::FreeRerolls },
		{ L"Trees", ESecondaryStats::Trees },
		{ L"Enemies", ESecondaryStats::Enemies },
		{ L"EnemySpeed", ESecondaryStats::EnemySpeed },
	};

	// 상점 카드 라벨로 쓰이므로 콜론까지 포함한다.
	const FWeaponStatEntry WeaponStatTable[] =
	{
		{ L"Damage:", EWeaponStats::Damage },
		{ L"Critical:", EWeaponStats::Critical },
		{ L"Cooldown:", EWeaponStats::Cooldown },
		{ L"Knockback:", EWeaponStats::Knockback },
		{ L"Range:", EWeaponStats::Range },
		{ L"Piercing:", EWeaponStats::Piercing },
	};

	const wchar_t* PrimaryIconPathTable[]
	{
		L"Custom/Stats/MaxHP.png",
		L"Custom/Stats/HPRegeneration.png",
		L"Custom/Stats/LifeSteal.png",
		L"Custom/Stats/Damage.png",
		L"Custom/Stats/MeleeDamage.png",
		L"Custom/Stats/RangedDamage.png",
		L"Custom/Stats/ElementalDamage.png",
		L"Custom/Stats/AttackSpeed.png",
		L"Custom/Stats/CritChance.png",
		L"Custom/Stats/Engineering.png",
		L"Custom/Stats/Range.png",
		L"Custom/Stats/Armor.png",
		L"Custom/Stats/Dodge.png",
		L"Custom/Stats/Speed.png",
		L"Custom/Stats/Luck.png",
		L"Custom/Stats/Harvesting.png",
	};
}

const wchar_t* GetStatName(EPrimaryStats Stat)
{
	for (const auto& Entry : PrimaryStatTable)
	{
		if (Entry.Value == Stat)
		{
			return Entry.Name;
		}
	}

	return nullptr;
}

const wchar_t* GetStatName(ESecondaryStats Stat)
{
	for (const auto& Entry : SecondaryStatTable)
	{
		if (Entry.Value == Stat)
		{
			return Entry.Name;
		}
	}

	return nullptr;
}

const wchar_t* GetStatName(EWeaponStats Stat)
{
	for (const auto& Entry : WeaponStatTable)
	{
		if (Entry.Value == Stat)
		{
			return Entry.Name;
		}
	}

	return nullptr;
}

const wchar_t* GetStatIcon(EPrimaryStats Stat)
{
	return PrimaryIconPathTable[(int)Stat];
}

const wchar_t* GetStatIcon(ESecondaryStats)
{
	return TEXT("Custom/Stats/Empty.png");
}

const wchar_t* GetStatIcon(const std::wstring& Name)
{
	EPrimaryStats PrimaryStat;

	if (FindStat(Name, PrimaryStat))
	{
		return GetStatIcon(PrimaryStat);
	}

	ESecondaryStats SecondaryStat;

	if (FindStat(Name, SecondaryStat))
	{
		return GetStatIcon(SecondaryStat);
	}

	return TEXT("Custom/Stats/Empty.png");;
}

bool FindStat(const std::wstring& Name, EPrimaryStats& Out)
{
	for (const auto& Entry : PrimaryStatTable)
	{
		if (Entry.Name == Name)
		{
			Out = Entry.Value;
			return true;
		}
	}

	return false;
}

bool FindStat(const std::wstring& Name, ESecondaryStats& Out)
{
	for (const auto& Entry : SecondaryStatTable)
	{
		if (Entry.Name == Name)
		{
			Out = Entry.Value;
			return true;
		}
	}

	return false;
}
