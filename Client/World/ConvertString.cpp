#include "ConvertString.h"

#include <cwchar>

std::wstring ToWString(const std::string& s, UINT codePage)
{
    if (s.empty()) return std::wstring();

    int len = MultiByteToWideChar(codePage, 0, s.data(), (int)s.size(), nullptr, 0);
    if (len <= 0) return std::wstring();

    std::wstring out(len, L'\0');
    MultiByteToWideChar(codePage, 0, s.data(), (int)s.size(), &out[0], len);
    return out;
}

std::string ToString(const std::wstring& w, UINT codePage)
{
    if (w.empty()) return std::string();

    int len = WideCharToMultiByte(codePage, 0, w.data(), (int)w.size(),
        nullptr, 0, nullptr, nullptr);
    if (len <= 0) return std::string();

    std::string out(len, '\0');
    WideCharToMultiByte(codePage, 0, w.data(), (int)w.size(),
        &out[0], len, nullptr, nullptr);
    return out;
}

std::wstring ConvertFloatToWString(float Value)
{
    wchar_t Buffer[32] = {};
    swprintf_s(Buffer, L"%.2f", Value);

    std::wstring out(Buffer);

    // 끝의 0을 지우고, 소수부가 전부 사라졌으면 소수점도 지운다.
    if (out.find(L'.') != std::wstring::npos)
    {
        while (out.back() == L'0') out.pop_back();
        if (out.back() == L'.') out.pop_back();
    }

    return out;
}