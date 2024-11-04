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

#include "sese/system/Environment.h"
#include "sese/Config.h"

#ifndef SESE_PLATFORM_WINDOWS
#include <stdlib.h>
#endif

namespace sese::system {

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

const char *Environment::getMajorVersion() noexcept {
    return SESE_MAJOR_VERSION;
}

const char *Environment::getMinorVersion() noexcept {
    return SESE_MINOR_VERSION;
}

const char *Environment::getPatchVersion() noexcept {
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

bool Environment::setEnv(const std::string &key, const std::string &value) noexcept {
#ifdef SESE_PLATFORM_WINDOWS
    return TRUE == SetEnvironmentVariable(key.c_str(), value.c_str());
#else
    return 0 == setenv(key.c_str(), value.c_str(), 1);
#endif
}

bool Environment::unsetEnv(const std::string &key) noexcept {
#ifdef SESE_PLATFORM_WINDOWS
    return TRUE == SetEnvironmentVariable(key.c_str(), nullptr);
#else
    return 0 == unsetenv(key.c_str());
#endif
}

std::string Environment::getEnv(const std::string &key) noexcept {
#ifdef SESE_PLATFORM_WINDOWS
    auto size = GetEnvironmentVariable(key.c_str(), nullptr, 0);
    if (size == 0) {
        return "";
    }
    auto str = std::make_unique<char[]>(size);
    GetEnvironmentVariable(key.c_str(), str.get(), size);
    return std::string(str.get(), size);
#else
#ifdef HAVE_SECURE_GETENV
    const char *value = secure_getenv(key.c_str());
#else
    const char *value = getenv(key.c_str());
#endif
    if (value == nullptr) {
        return "";
    } else {
        return value;
    }
#endif
}

} // namespace sese::system