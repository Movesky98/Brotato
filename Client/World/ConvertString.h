#pragma once
#include <string>
#include <windows.h>

std::wstring ToWString(const std::string& s, UINT codePage = CP_UTF8);
std::string ToString(const std::wstring& w, UINT codePage = CP_UTF8);
