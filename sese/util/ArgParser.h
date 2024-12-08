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
 * @file ArgParser.h
 * @brief Command line argument parsing class
 * @author kaoru
 * @date February 22, 2023
 * @version 0.2
 */

#pragma once

#include "sese/Config.h"

#include <map>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese {

/**
 * @brief Command line argument parsing class
 */
class  ArgParser {
public:
    typedef std::unique_ptr<ArgParser> Ptr;

public:
    ArgParser() = default;

    /**
     * Initialize parser
     * @param argc Number of arguments
     * @param argv Actual arguments
     * @return Whether parsing was successful
     */
    bool parse(int32_t argc, char **argv) noexcept;

    /**
     * @return Returns the entire parameter map
     */
    [[nodiscard]] const std::map<std::string, std::string> &getKeyValSet() const noexcept;

    /**
     * Get the parameter value by parameter name
     * @param key Parameter name
     * @param default_value Default parameter value
     * @return Returns the parameter value, or the default value if the parameter does not exist
     */
    [[nodiscard]] const std::string &getValueByKey(const std::string &key, const std::string &default_value) const noexcept;

    /**
     * Retrieves the value associated with the given key.
     * @param key The name of the parameter.
     * @return The value of the parameter if it exists.
     * @exception std::out_of_range If the key does not exist.
     */
    const std::string &getValueByKey(const std::string &key) const;

    /**
     * Determine whether the current parameter exists based on the parameter name, applicable for some individual switches that do not need specified values
     * @param key Parameter name
     * @return Whether the parameter exists
     */
    [[nodiscard]] bool exist(const std::string &key) const noexcept;

    /**
     * Get the program working directory
     * @return Working directory string, without a trailing directory separator
     */
    [[nodiscard]] const std::string &getCurrentPath() const { return currentPath; }

    /**
     * Get the current program name
     * @return Current program name
     */
    [[nodiscard]] const std::string &getFileName() const { return fileName; }

    /**
     * Get the current absolute path of the program
     * @return Program path
     */
    [[nodiscard]] const std::string &getFullPath() const { return fullPath; }

private:
    std::string fullPath;
    std::string currentPath;
    std::string fileName;
    std::map<std::string, std::string> keyValSet;
};

} // namespace sese
