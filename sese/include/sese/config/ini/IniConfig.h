#pragma once

#include <sese/Config.h>

#include <map>

#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif

namespace sese::config::ini {

    struct API IniConfig {
    public:
        using Section = std::map<std::string, std::string>;
        using SectionMap = std::map<std::string, Section>;

        Section defSection;
        SectionMap sectionMap;
    };

}// namespace sese::config::ini