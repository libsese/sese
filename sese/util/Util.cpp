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

#include <sese/util/Util.h>
#include <sese/util/SymbolConverter.h>

#include <cstring>

#ifndef SESE_PLATFORM_WINDOWS
#include <unistd.h>
#endif

int64_t sese::toInteger(const std::string &string, int radix) {
    char *end;
    return std::strtol(string.c_str(), &end, radix);
}

std::string sese::getClassName(const std::type_info *type) {
#ifdef _MSC_VER
    return SymbolConverter::decodeMSVCClassName(type);
#else
    return SymbolConverter::decodeUnixClassName(type);
#endif
}

bool sese::isSpace(char ch) noexcept {
    auto p = SPACE_CHARS;
    while (*p != '\0') {
        if (*p == ch) {
            return true;
        }
        p++;
    }
    return false;
}

int32_t sese::findFirstAt(const char *str, char ch) {
    const char *pos = str;
    int32_t at = 0;
    while (*pos != '\0') {
        if (*pos == ch) {
            return at;
        } else {
            pos++;
            at++;
        }
    }
    return -1;
}

int sese::StrCmp::operator()(char const *lv, char const *rv) const {
    return ::strcmp(lv, rv);
}

int sese::StrCmp::operator()(const std::string &lv, const std::string &rv) const {
    return this->operator()(lv.c_str(), rv.c_str());
}

int sese::StrCmpI::operator()(char const *lv, char const *rv) const {
    return strcmpi(lv, rv);
}

int sese::StrCmpI::operator()(const std::string &lv, const std::string &rv) const {
    return this->operator()(lv.c_str(), rv.c_str());
}

bool sese::strcmp(const char *lv, const char *rv) noexcept {
    return ::strcmp(lv, rv) == 0;
}

bool sese::strcmpDoNotCase(const char *lv, const char *rv) noexcept {
    return strcmpi(lv, rv) == 0;
}

void sese::sleep(uint32_t second) {
#ifdef _WIN32
    ::Sleep(second * 1000);
#else
    ::sleep(second);
#endif
}

std::string sese::getErrorString(int64_t error) {
#ifdef _WIN32
    char buffer[255];
    strerror_s(buffer, 255, (int) error); // NOLINT
    return {buffer};
#else
    return strerror(error); // GCOVR_EXCL_LINE
#endif
}

int32_t sese::getErrorCode() {
#ifdef _WIN32
    return GetLastError();
#else
    return errno;
#endif
}

size_t sese::streamMove(sese::io::OutputStream *out, sese::io::InputStream *in, size_t size) noexcept {
    char buffer[4096];
    auto expect = size;
    while (size) {
        auto need = size >= 4096 ? 4096 : size;
        auto len = in->read(buffer, need);
        out->write(buffer, len);
        size -= len;
        if (len != need) {
            return expect - size;
        }
    }
    return expect;
}

const char *getSpecificVersion() {
    return SESE_MAJOR_VERSION "." SESE_MINOR_VERSION "." SESE_PATCH_VERSION;
}