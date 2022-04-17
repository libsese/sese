#include "system/Environment.h"

namespace sese {

    bool Environment::isLittleEndian() noexcept {
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) || defined(_WIN32)
        return true;
#else
        return false;
#endif
    }

    const char *Environment::getRepoHash() noexcept {
        return SESE_REPO_HASH;
    }

    const char *Environment::getRepoBranch() noexcept {
        return SESE_REPO_BRANCH;
    }

    int Environment::getMajorVersion() noexcept {
        return SESE_MAJOR_VERSION;
    }

    int Environment::getMinorVersion() noexcept {
        return SESE_MINOR_VERSION;
    }

    long Environment::getPatchVersion() noexcept {
        return SESE_PATCH_VERSION;
    }

    const char *Environment::getBuildDate() noexcept {
        return __DATE__;
    }

    const char *Environment::getBuildTime() noexcept {
        return __TIME__;
    }

    const char *Environment::getBuildDateTime() noexcept {
        return __TIMESTAMP__;
    }

    const char *Environment::getOperateSystemType() noexcept {
#ifdef _WIN32
        return "Windows";
#elif __APPLE__
#include "TargetConditionals.h"
#if TARGET_IPHONE_SIMULATOR
        return "iOS_Simulator";
#elif TARGET_OS_IPHONE
        return "iOS_Device";
#elif TARGET_OS_MAC
        return "Macos";
#endif
#elif __ANDROID__
        return "Android";
#elif __linux__
        return "Linux";
#elif __unix__
        return "Unix";
#else
        return "Other OS"
#endif
    }
}// namespace sese