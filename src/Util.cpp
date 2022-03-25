#include "Util.h"
#include "Config.h"
#include <cstring>

namespace sese {

    [[maybe_unused]] static struct InitStruct {
        InitStruct() {
            // 初始化 Logger
            auto logger = sese::Singleton<Logger>::getInstance();
            auto formatter = std::make_shared<SimpleFormatter>();
            auto appender = std::make_shared<ConsoleAppender>(formatter);
            logger->addAppender(appender);
        }
        ~InitStruct() = default;
    } initStruct; /* NOLINT */

    Logger *getLogger() noexcept {
        return sese::Singleton<Logger>::getInstance();
    }

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

}// namespace sese
