#include "CSVData.h"

#include "StatTable.h"

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
		{L"SpawnSlash", ESkillComponent::SpawnSlash},
		{L"SpawnEnemy", ESkillComponent::SpawnEnemy}
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

bool ConvertWideToMovement(const std::wstring& In, std::vector<FMovementType>& Out)
{
	std::wstringstream WSS(In);
	std::vector<std::wstring> Phases;
	std::wstring Token;

	// 보스의 경우 '>'를 기준으로 페이즈를 나눈다.
	while (std::getline(WSS, Token, L'>'))
	{
		Phases.push_back(Token);
	}

	for (auto Phase : Phases)
	{
		wchar_t Delimiter = L'|';
		size_t Pos = Phase.find(Delimiter);
		std::wstring InRangeStr;
		std::wstring OutRangeStr;

		FMovementType MovementType;
		
		if (Pos != std::wstring::npos)
		{
			InRangeStr = Phase.substr(0, Pos);
			OutRangeStr = Phase.substr(Pos + 1);
		}
		else
		{
			InRangeStr = Phase;
			OutRangeStr = Phase;
		}

		for (const auto& Element : MovementTable)
		{
			if (Element.Name == InRangeStr)
			{
				MovementType.InRange = Element.Value;
			}

			if (Element.Name == OutRangeStr)
			{
				MovementType.OutRange = Element.Value;
			}
		}

		Out.push_back(MovementType);
	}

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
			std::wstring PrimaryStatWStr = Stat.substr(0, Pos);
			std::wstring Value = Stat.substr(Pos + 1);

			EPrimaryStats PrimaryStat; 
			if (FindStat(PrimaryStatWStr, PrimaryStat))
			{
				Out.Primary[(int)PrimaryStat] = std::stoi(Value);
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
			std::wstring SecondaryStatWStr = Stat.substr(0, Pos);
			std::wstring Value = Stat.substr(Pos + 1);

			ESecondaryStats SecondaryStat;
			if (FindStat(SecondaryStatWStr, SecondaryStat))
			{
				Out.Secondary[(int)SecondaryStat] = std::stoi(Value);
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
