#include "convert/EncodingConverter.h"
#include <codecvt>
#include <locale>

static std::wstring_convert<std::codecvt_utf8<wchar_t>> convert; /* NOLINT */

std::string sese::EncodingConverter::toString(const std::wstring &wstring) noexcept {
    return convert.to_bytes(wstring.c_str());
}

std::wstring sese::EncodingConverter::toWstring(const std::string &string) noexcept {
    return convert.from_bytes(string.c_str());
}