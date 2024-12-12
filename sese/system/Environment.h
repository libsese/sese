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

/**
 * @file Environment.h
 * @brief Environment Information Class
 * @author kaoru
 * @date March 28, 2022
 */

#pragma once

#include "sese/util/NotInstantiable.h"

#include <string>

#ifdef _WIN32
#pragma warning(disable : 4624)
#endif

namespace sese::system {

/**
 * @brief Environment Information Class
 */
class Environment final : public NotInstantiable {
public:
    Environment() = delete;

    /**
     * @return Returns the first 6 bytes of the repository hash
     */
    static const char *getRepoHash() noexcept;

    /**
     * @return Return to the branch where the repository is located
     */
    static const char *getRepoBranch() noexcept;

    /**
     * @return Returns the major version number
     */
    static const char *getMajorVersion() noexcept;

    /**
     * @return Returns the minor version number
     */
    static const char *getMinorVersion() noexcept;

    /**
     * @return Returns the patch version number
     */
    static const char *getPatchVersion() noexcept;

    /**
     * @return Returns the build date string
     */
    static const char *getBuildDate() noexcept;

    /**
     * @return Returns the build time string
     */
    static const char *getBuildTime() noexcept;

    /**
     * @return Returns a timestamp of the build time
     */
    static const char *getBuildDateTime() noexcept;

    /**
     * @return Returns the type of operating system that was built
     */
    static const char *getOperateSystemType() noexcept;

    /**
     * @return Returns the CPU's big-and-small mode
     */
    static bool isLittleEndian() noexcept;

    /**
     * Set the current context environment variable
     * @param key Key
     * @param value Value
     * @return Whether the setting was successful
     */
    static bool setEnv(const std::string &key, const std::string &value) noexcept;

    /**
     * Unset the current context environment variable
     * @param key Key
     * @return Whether the unset operation was successful
     */
    static bool unsetEnv(const std::string &key) noexcept;

    /**
     * Get the current context environment variable
     * @param key Key
     * @return The retrieved value, which needs to be checked with std::string::empty() for emptiness
     */
    static std::string getEnv(const std::string &key) noexcept;
};
} // namespace sese::system