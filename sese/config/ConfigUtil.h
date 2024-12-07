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
 * @file ConfigUtil.h
 * @brief Traditional configuration file parsing utility
 * @author Kaoru
 * @date June 17, 2023
 * @version 0.1.0
 */

#pragma once

#include "sese/io/InputStream.h"
#include "sese/io/OutputStream.h"

#include <map>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese {

/**
 * @brief Traditional configuration file class
 */
class ConfigObject {
public:
    using Ptr = std::shared_ptr<ConfigObject>;

    /// Section
    struct Section {
        using Ptr = std::shared_ptr<Section>;

        std::string name;
        std::map<std::string, std::string> parameter{};

        explicit Section(std::string name);
        /**
         * Retrieves the value based on the key name
         * @param key The key name
         * @param default_value The default value
         * @return The corresponding value if the key exists, otherwise returns the default value
         */
        [[nodiscard]] std::string getValueByKey(const std::string &key, const std::string &default_value) const;
        void setKeyValue(const std::string &key, const std::string &value);
    };

    ConfigObject();
    [[nodiscard]] Section::Ptr getDefaultSection() const noexcept { return this->defaultSection; } // GCOVR_EXCL_LINE
    /**
     * Retrieves the section based on the section name
     * @param section_name The name of the section
     * @return Returns a pointer to the section if it exists, otherwise returns nullptr
     */
    [[nodiscard]] Section::Ptr getSectionByName(const std::string &section_name) const;
    void setSection(const Section::Ptr &section);
    [[nodiscard]] const std::map<std::string, Section::Ptr> &getAllSection() const;

private:
    Section::Ptr defaultSection;
    std::map<std::string, Section::Ptr> sections;
};

/**
 * @brief Traditional configuration file utility
 */
class SESE_DEPRECATED ConfigUtil {
public:
    using InputStream = io::InputStream;
    using OutputStream = io::OutputStream;

    /**
     * Reads the configuration file
     * @param input The input stream of the configuration file
     * @return A pointer to the configuration file class, returns nullptr if reading fails
     */
    static ConfigObject::Ptr readFrom(InputStream *input);
    /**
     * Writes the configuration to the file
     * @param config_file The specific configuration
     * @param output The output stream of the configuration file
     * @return Whether the write operation was successful
     */
    static bool write2(const ConfigObject::Ptr &config_file, OutputStream *output);

private:
    static std::string readLine(InputStream *input);
};
} // namespace sese