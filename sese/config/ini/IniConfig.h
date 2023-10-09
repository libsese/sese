/**
 * \file IniConfig.h
 * \author kaoru
 * \brief INI 配置类
 * \date 2023年9月13日
 */

#pragma once

#include <sese/Config.h>

#include <map>

#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif

namespace sese::ini {

/// INI 配置类
struct API IniConfig {
public:
    /// INI SECTION
    using Section = std::map<std::string, std::string>;
    /// INI SECTION MAPPING
    using SectionMap = std::map<std::string, Section>;

    /// 默认节
    Section defSection;
    /// 节映射
    SectionMap sectionMap;
};

} // namespace sese::ini