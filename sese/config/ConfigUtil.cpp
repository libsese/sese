#include "sese/config/ConfigUtil.h"
#include "sese/config/UniReader.h"
#include <regex>

namespace sese {

    ConfigFile::Section::Section(std::string name) : name(std::move(name)) {
    }

    std::string ConfigFile::Section::getValueByKey(const std::string &key, const std::string &defaultValue) {
        for (const auto &itor: parameter) {
            if (itor.first == key) {
                return itor.second;
            }
        }
        return defaultValue;
    }

    void ConfigFile::Section::setKeyValue(const std::string &key, const std::string &value) {
        parameter[key] = value;
    }

    ConfigFile::ConfigFile() {
        defaultSection = std::make_shared<ConfigFile::Section>("default");
    }

    ConfigFile::Section::Ptr ConfigFile::getSectionByName(const std::string &sectionName) {
        for (const auto &itor: sections) {
            if (itor.first == sectionName) {
                return itor.second;
            }
        }
        return nullptr;
    }

    void ConfigFile::setSection(const Section::Ptr &section) {
        sections[section->name] = section;
    }

    const std::map<std::string, ConfigFile::Section::Ptr> &ConfigFile::getAllSection() const {
        return sections;
    }

    ConfigFile::Ptr ConfigUtil::readFrom(const std::string &fileName) {
        ConfigFile::Ptr configFile = std::make_shared<ConfigFile>();
        UniReader::Ptr reader = std::make_shared<UniReader>();
        if(!reader->open(fileName)) return nullptr;
        ConfigFile::Section::Ptr currentSection = configFile->getDefaultSection();

        std::regex commentRegex(R"(\s*\;.*)");
        std::regex sectionRegex(R"(\[(\S+)\])");
        std::regex parameterRegex1(R"(\s*(\S+)\s*\=\s*\"(.+)\"\s*)");
        std::regex parameterRegex2(R"(\s*(\S+)\s*\=\s*([^"]+)\s*)");

        while (true) {
            auto line = reader->readLine();
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
                    auto section = std::make_shared<ConfigFile::Section>(sectionName);
                    configFile->setSection(section);
                    currentSection = section;
                } else {
                    continue;
                }
            }
        }
        return configFile;
    }

    inline void writePair(const FileStream::Ptr& fileStream, const std::pair<const std::string, std::string> &pair){
        fileStream->write((void *)pair.first.c_str(), pair.first.length());
        fileStream->write((void *)"=\"", 2);
        fileStream->write((void *)pair.second.c_str(), pair.second.length());
        fileStream->write((void *)"\"\n", 2);
    }

    bool ConfigUtil::write2(const ConfigFile::Ptr &configFile, const std::string &fileName) {
        FileStream::Ptr fileStream = FileStream::create(fileName, TEXT_WRITE_CREATE_TRUNC);
        if(!fileStream) return false;

        for(const auto& itor : configFile->getDefaultSection()->parameter){
            writePair(fileStream, itor);
        }

        for(const auto&section: configFile->getAllSection()){
            fileStream->write((void *)"[", 1);
            fileStream->write((void *) section.first.c_str(), section.first.length());
            fileStream->write((void *)"]\n", 2);
            for(const auto& itor : section.second->parameter) {
                writePair(fileStream, itor);
            }
        }

        fileStream->close();
        return true;
    }

}// namespace sese