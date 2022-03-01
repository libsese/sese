#include "Util.h"
#include "Config.h"

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

    const char *getLevelString(Level level) noexcept {
        switch(level){
            case Level::DEBUG:
                return "DEBUG";
            case Level::INFO:
                return "INFO ";
            case Level::WARN:
                return "WARN ";
            case Level::ERROR:
                return "ERROR";
            default:
                return "DEBUG ";
        }
    }
}// namespace sese
