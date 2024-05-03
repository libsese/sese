#include "sese/util/Util.h"

#include <cstring>

#ifndef _WIN32
#include <unistd.h>
#endif

int64_t sese::toInteger(const std::string &string, int radix) {
    char *end;
    return std::strtol(string.c_str(), &end, radix);
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

int sese::StrCmp::operator()(char const *lv, char const *rv) {
    return ::strcmp(lv, rv);
}

int sese::StrCmpI::operator()(char const *lv, char const *rv) {
    return strcmpi(lv, rv);
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

int64_t sese::getErrorCode() {
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