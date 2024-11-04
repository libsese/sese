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
#include <codecvt>
#include <locale>

// GCOVR_EXCL_START

static std::wstring_convert<std::codecvt_utf8<wchar_t>> *convert; /* NOLINT */

std::string sese::EncodingConverter::toString(const std::wstring &wstring) noexcept {
    return convert->to_bytes(wstring.c_str());
}

std::wstring sese::EncodingConverter::toWstring(const std::string &string) noexcept {
    return convert->from_bytes(string.c_str());
}

int32_t sese::EncodingConverterInitiateTask::init() noexcept {
    convert = new std::wstring_convert<std::codecvt_utf8<wchar_t>>;
    return 0;
}

int32_t sese::EncodingConverterInitiateTask::destroy() noexcept {
    delete convert;
    return 0;
}

// GCOVR_EXCL_STOP