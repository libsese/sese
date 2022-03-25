#pragma once
#include "Config.h"
#include <string>

#ifdef _WIN32
#define WILL_SKIP 1
#endif

#ifdef __linux__
#define WILL_SKIP 2
#endif

#ifdef __APPLE__
#define WILL_SKIP 2
#endif

namespace sese {
    extern API std::string backtrace2String(int size, int skip, const std::string &prefix);
}