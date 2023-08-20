#include "sese/util/Util.h"

#include <cstring>

#ifndef _WIN32
#include <unistd.h>
#endif

namespace sese {

    bool isSpace(char ch) noexcept {
        auto p = SPACE_CHARS;
        while (*p != '\0') {
            if (*p == ch) {
                return true;
            }
            p++;
        }
        return false;
    }

    int32_t findFirstAt(const char *str, char ch) {
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

    bool StrCmp::operator()(char const *lv, char const *rv) {
        return ::strcmp(lv, rv);
    }

    bool StrCmpI::operator()(char const *lv, char const *rv) {
        return strcmpi(lv, rv);
    }

    bool strcmp(const char *lv, const char *rv) noexcept {
        return ::strcmp(lv, rv) == 0;
    }

    bool strcmpDoNotCase(const char *lv, const char *rv) noexcept {
        return strcmpi(lv, rv) == 0;
    }

    void sleep(uint32_t second) {
#ifdef _WIN32
        ::Sleep(second * 1000);
#else
        ::sleep(second);
#endif
    }

    std::string getErrorString(int64_t error) {
#ifdef _WIN32
        char buffer[255];
        strerror_s(buffer, 255, (int) error);// NOLINT
        return {buffer};
#else
        return strerror(error);// GCOVR_EXCL_LINE
#endif
    }

    int64_t getErrorCode() {
#ifdef _WIN32
        return GetLastError();
#else
        return errno;
#endif
    }

    size_t streamMove(sese::OutputStream *out, sese::InputStream *in, size_t size) noexcept {
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

}// namespace sese

const char *getSpecificVersion() {
    return SESE_MAJOR_VERSION "." SESE_MINOR_VERSION "." SESE_PATCH_VERSION;
}