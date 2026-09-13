#pragma once
#include <string>
#include <windows.h>

std::wstring ToWString(const std::string& s, UINT codePage = CP_UTF8);
std::string ToString(const std::wstring& w, UINT codePage = CP_UTF8);

// 소수 둘째 자리까지 반올림하고 끝의 0과 소수점을 지운다. (0.167 -> 0.17, 2.0 -> 2)
std::wstring ConvertFloatToWString(float Value);
