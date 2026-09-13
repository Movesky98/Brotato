#pragma once

#include "EngineInfo.h"
#include "GameTypes.h"

const wchar_t* GetStatName(EPrimaryStats Stat);
const wchar_t* GetStatName(ESecondaryStats Stat);
const wchar_t* GetStatName(EWeaponStats Stat);

const wchar_t* GetStatIcon(EPrimaryStats Stat);
const wchar_t* GetStatIcon(ESecondaryStats Stat);
const wchar_t* GetStatIcon(const std::wstring& Name);

bool FindStat(const std::wstring& Name, EPrimaryStats& Out);
bool FindStat(const std::wstring& Name, ESecondaryStats& Out);