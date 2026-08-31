#include "CSVData.h"

namespace {
	struct FMovementEntry
	{
		const wchar_t* Name;
		EEnemyMovement Value;
	};

	constexpr FMovementEntry MovementTable[] =
	{
		{L"None", EEnemyMovement::None},
		{L"ChaseTarget", EEnemyMovement::ChaseTarget},
		{L"FleeFromTarget", EEnemyMovement::FleeFromTarget},
		{L"Random", EEnemyMovement::Random},
	};
	
	struct FTriggerEntry
	{
		const wchar_t* Name;
		ESkillTrigger Value;
	};

	constexpr FTriggerEntry TriggerTable[] =
	{
		{L"OnHit", ESkillTrigger::OnHit},
		{L"InPerceptionRange", ESkillTrigger::InPerceptionRange},
		{L"OnDeath", ESkillTrigger::OnDeath},
		{L"Always", ESkillTrigger::Always},
	};

	struct FSkillComponentEntry
	{
		const wchar_t* Name;
		ESkillComponent Value;
	};

	constexpr FSkillComponentEntry SkillComponentTable[] =
	{
		{L"Dash", ESkillComponent::Dash},
		{L"SpawnProjectile", ESkillComponent::SpawnProjectile},
	};

	struct FSkillTargetEntry
	{
		const wchar_t* Name;
		ESkillTarget Value;
	};

	constexpr FSkillTargetEntry SkillTargetTable[] =
	{
		{L"None", ESkillTarget::None},
		{L"Target", ESkillTarget::Target},
		{L"Owner", ESkillTarget::Owner},
	};

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

	struct FWeaponTypeEntry
	{
		const wchar_t* Name;
		EWeaponType Value;
	};

	const FWeaponTypeEntry WeaponTypeTable[] =
	{
		{L"Ranged", EWeaponType::Ranged},
		{L"Melee", EWeaponType::Melee},
	};
}

bool ConvertWideToUTF8(const std::wstring& In, std::string& Out)
{
	if (In.empty()) return false;
	int Len = WideCharToMultiByte(CP_UTF8, 0, In.data(), (int)In.size(), nullptr, 0, nullptr, nullptr);
	std::string Result(Len, '\0');
	WideCharToMultiByte(CP_UTF8, 0, In.data(), (int)In.size(), Result.data(), Len, nullptr, nullptr);
	
	Out = Result;
	return true;
}

bool ConvertWideToMovement(const std::wstring& In, std::vector<EEnemyMovement>& Out)
{
	std::vector<EEnemyMovement> MovementTypes;
	std::wstringstream WSS(In);
	std::wstring Token;

	while (std::getline(WSS, Token, L'|'))
	{
		for(const auto& Element : MovementTable)
		{
			if (Element.Name == Token)
			{
				MovementTypes.push_back(Element.Value);
				break;
			}
		}
	}

	Out = MovementTypes;
	return true;
}

bool ConvertWideToTrigger(const std::wstring& In, ESkillTrigger& Out)
{
	for (const auto& Element : TriggerTable)
	{
		if (Element.Name == In)
		{
			Out = Element.Value;
			return true;
		}
	}

	return false;
}

bool SplitDataToSkills(const std::wstring& In, std::vector<std::wstring>& Out)
{
	std::vector<std::wstring> SkillIDs;
	std::wstringstream WSS(In);
	std::wstring Token;

	while (std::getline(WSS, Token, L'|'))
	{
		SkillIDs.push_back(Token);
	}

	Out = SkillIDs;

	return true;
}

bool ConvertWideToSkillComponent(const std::wstring& In, ESkillComponent& Out)
{
	for (const auto& Element : SkillComponentTable)
	{
		if (In == Element.Name)
		{
			Out = Element.Value;
			return true;
		}
	}

	return false;
}

bool ConvertWideToSkillTarget(const std::wstring& In, ESkillTarget& Out)
{
	for (const auto& Element : SkillTargetTable)
	{
		if (Element.Name == In)
		{
			Out = Element.Value;
			return true;
		}
	}
	
	return false;
}

bool ConvertWideToPrimaryStats(const std::wstring& In, FStat& Out)
{
	std::vector<std::wstring> Stats;
	std::wstringstream WSS(In);
	std::wstring Token;

	while (std::getline(WSS, Token, L'|'))
	{
		Stats.push_back(Token);
	}

	for (auto Stat : Stats)
	{
		wchar_t Delimiter = L':';
		size_t Pos = Stat.find(Delimiter);

		if (Pos != std::wstring::npos)
		{
			std::wstring PrimaryStat = Stat.substr(0, Pos);
			std::wstring Value = Stat.substr(Pos + 1);

			for (const auto& Element : PrimaryStatTable)
			{
				if (Element.Name == PrimaryStat)
				{
					Out.Primary[(int)Element.Value] = std::stoi(Value);
					break;
				}
			}
		}
	}

	return true;
}

bool ConvertWideToSecondaryStats(const std::wstring& In, FStat& Out)
{
	std::vector<std::wstring> Stats;
	std::wstringstream WSS(In);
	std::wstring Token;

	while (std::getline(WSS, Token, L'|'))
	{
		Stats.push_back(Token);
	}

	for (auto Stat : Stats)
	{
		wchar_t Delimiter = L':';
		size_t Pos = Stat.find(Delimiter);

		if (Pos != std::wstring::npos)
		{
			std::wstring Secondary = Stat.substr(0, Pos);
			std::wstring Value = Stat.substr(Pos + 1);

			for (const auto& Element : SecondaryStatTable)
			{
				if (Element.Name == Secondary)
				{
					Out.Secondary[(int)Element.Value] = std::stoi(Value);
					break;
				}
			}
		}
	}

	return true;
}

bool ConvertWideToWeaponType(const std::wstring& In, EWeaponType& Out)
{
	for (const auto& Element : WeaponTypeTable)
	{
		if (Element.Name == In)
		{
			Out = Element.Value;
			return true;
		}
	}

	return false;
}
