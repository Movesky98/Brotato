#include "ConvertString.h"

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