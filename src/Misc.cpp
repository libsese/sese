#include "Misc.h"

static const char *SpaceChar = " \n\r\t\v\b";

bool isSpace(char ch) noexcept {
    auto p = SpaceChar;
    while (*p != '\0') {
        if (*p == ch) {
            return true;
        }
        p++;
    }
    return false;
}