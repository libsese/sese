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
        return strcmp(lv, rv);
    }

    bool StrCmpI::operator()(char const *lv, char const *rv) {
        return strcmpi(lv, rv);
    }

    void sleep(uint32_t second) {
#ifdef _WIN32
        ::Sleep(second * 1000);
#else
        ::sleep(second);
#endif
    }

    std::string getErrorString(int32_t error){
#ifdef _WIN32
        char buffer[255];
        strerror_s(buffer, 255, error);
        return { buffer };
#else
        return strerror(error);
#endif
    }

}// namespace sese

const char *getSpecificVersion() {
    return SESE_MAJOR_VERSION "." SESE_MINOR_VERSION "." SESE_REPO_HASH;
}