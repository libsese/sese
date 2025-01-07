// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "sese/convert/EncodingConverter.h"
#include "sese/text/String.h"

std::string sese::EncodingConverter::toString(const std::wstring &wstring) noexcept {
    auto str = text::String::fromUCS2LE(wstring.c_str());
    return str.toString();
}

std::wstring sese::EncodingConverter::toWstring(const std::string &string) noexcept {
    auto str = text::String::fromUTF8(string.c_str());
    return str.toWString();
}
