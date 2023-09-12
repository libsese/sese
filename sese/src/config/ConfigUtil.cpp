#include "sese/config/ConfigUtil.h"

#include <sstream>
#include <regex>

namespace sese {

    ConfigObject::Section::Section(std::string name) : name(std::move(name)) {
    }

    std::string ConfigObject::Section::getValueByKey(const std::string &key, const std::string &defaultValue) {
        for (const auto &iterator: parameter) {
            if (iterator.first == key) {
                return iterator.second;
            }
        }
        return defaultValue;
    }

    void ConfigObject::Section::setKeyValue(const std::string &key, const std::string &value) {
        parameter[key] = value;
    }

    ConfigObject::ConfigObject() {
        defaultSection = std::make_shared<ConfigObject::Section>("default");
    }

    ConfigObject::Section::Ptr ConfigObject::getSectionByName(const std::string &sectionName) {
        for (const auto &iterator: sections) {
            if (iterator.first == sectionName) {
                return iterator.second;
            }
        }
        return nullptr;
    }

    void ConfigObject::setSection(const Section::Ptr &section) {
        sections[section->name] = section;
    }

    const std::map<std::string, ConfigObject::Section::Ptr> &ConfigObject::getAllSection() const {
        return sections;
    }

    ConfigObject::Ptr ConfigUtil::readFrom(io::InputStream *input) {
        ConfigObject::Ptr configFile = std::make_shared<ConfigObject>();
        ConfigObject::Section::Ptr currentSection = configFile->getDefaultSection();

        std::regex commentRegex(R"(\s*\;.*)");
        std::regex sectionRegex(R"(\[(\S+)\])");
        std::regex parameterRegex1(R"(\s*(\S+)\s*\=\s*\"(.+)\"\s*)");
        std::regex parameterRegex2(R"(\s*(\S+)\s*\=\s*([^"]+)\s*)");

        while (true) {
            auto line = readLine(input);
            if (line.empty()) {
                break;
            } else {
                std::match_results<std::string::iterator> matchResults;
                if (std::regex_match(line, commentRegex)) {
                    continue;
                } else if (std::regex_search(line.begin(), line.end(), matchResults, parameterRegex1) || std::regex_search(line.begin(), line.end(), matchResults, parameterRegex2)) {
                    auto key = matchResults[1].str();
                    auto value = matchResults[2].str();
                    currentSection->setKeyValue(key, value);
                } else if (std::regex_search(line.begin(), line.end(), matchResults, sectionRegex)) {
                    auto sectionName = matchResults[1].str();
                    auto section = std::make_shared<ConfigObject::Section>(sectionName);
                    configFile->setSection(section);
                    currentSection = section;
                } else {
                    continue;
                }
            }
        }
        return configFile;
    }

    inline void writePair(io::OutputStream *output, const std::pair<const std::string, std::string> &pair) {
        output->write((void *) pair.first.c_str(), pair.first.length());
        output->write((void *) "=\"", 2);
        output->write((void *) pair.second.c_str(), pair.second.length());
        output->write((void *) "\"\n", 2);
    }

    bool ConfigUtil::write2(const ConfigObject::Ptr &configFile, io::OutputStream *output) {
        for (const auto &iterator: configFile->getDefaultSection()->parameter) {
            writePair(output, iterator);
        }

        for (const auto &section: configFile->getAllSection()) {
            output->write((void *) "[", 1);
            output->write((void *) section.first.c_str(), section.first.length());
            output->write((void *) "]\n", 2);
            for (const auto &iterator: section.second->parameter) {
                writePair(output, iterator);
            }
        }

        return true;
    }

    std::string ConfigUtil::readLine(io::InputStream *input) {
        size_t count = 0;
        char ch;
        std::stringstream stream;
        while (true) {
            auto success = input->read(&ch, 1);
            if (success < 1) {
                return stream.str();
            } else if (ch == '\r') {
                continue;
            } else if (ch == '\n') {
                if (count > 0) {
                    return stream.str();
                } else {
                    continue;
                }
            } else {
                stream << ch;
                count += 1;
            }
        }
    }

}// namespace sese