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
 * \file IniConfig.h
 * \author kaoru
 * \brief INI Configuration Class
 * \date September 13, 2023
 */

#pragma once

#include <sese/Config.h>

#include <map>

#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif

namespace sese::ini {

/// INI Configuration Class
struct  IniConfig {
public:
    /// INI SECTION
    using Section = std::map<std::string, std::string>;
    /// INI SECTION MAPPING
    using SectionMap = std::map<std::string, Section>;

    Section defSection;
    SectionMap sectionMap;
};

} // namespace sese::ini