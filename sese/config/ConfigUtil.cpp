#include "sese/config/ConfigUtil.h"
#include "sese/text/StringBuilder.h"

#include <regex>

namespace sese {

ConfigObject::Section::Section(std::string name) : name(std::move(name)) {
}

std::string ConfigObject::Section::getValueByKey(const std::string &key, const std::string &default_value) const {
    for (const auto &[fst, snd]: parameter) {
        if (fst == key) {
            return snd;
        }
    }
    return default_value;
}

void ConfigObject::Section::setKeyValue(const std::string &key, const std::string &value) {
    parameter[key] = value;
}

ConfigObject::ConfigObject() {
    defaultSection = std::make_shared<ConfigObject::Section>("default");
}

ConfigObject::Section::Ptr ConfigObject::getSectionByName(const std::string &section_name) const {
    for (const auto &[fst, snd]: sections) {
        if (fst == section_name) {
            return snd;
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
    auto config_file = std::make_shared<ConfigObject>();
    auto current_section = config_file->getDefaultSection();

    std::regex comment_regex(R"(\s*\;.*)");
    std::regex section_regex(R"(\[(\S+)\])");
    std::regex parameter_regex1(R"(\s*(\S+)\s*\=\s*\"(.+)\"\s*)");
    std::regex parameter_regex2(R"(\s*(\S+)\s*\=\s*([^"]+)\s*)");

    while (true) {
        if (auto line = readLine(input); line.empty()) {
            break;
        } else {
            if (std::regex_match(line, comment_regex))
                continue;
            if (std::match_results<std::string::iterator> match_results; std::regex_search(line.begin(), line.end(), match_results, parameter_regex1) || std::regex_search(line.begin(), line.end(), match_results, parameter_regex2)) {
                auto key = match_results[1].str();
                auto value = match_results[2].str();
                current_section->setKeyValue(key, value);
            } else if (std::regex_search(line.begin(), line.end(), match_results, section_regex)) {
                auto section_name = match_results[1].str();
                auto section = std::make_shared<ConfigObject::Section>(section_name);
                config_file->setSection(section);
                current_section = section;
            } else {
                continue;
            }
        }
    }
    return config_file;
}

inline void writePair(io::OutputStream *output, const std::pair<const std::string, std::string> &pair) {
    output->write(pair.first.c_str(), pair.first.length());
    output->write("=\"", 2);
    output->write(pair.second.c_str(), pair.second.length());
    output->write("\"\n", 2);
}

bool ConfigUtil::write2(const ConfigObject::Ptr &config_file, io::OutputStream *output) {
    for (const auto &iterator: config_file->getDefaultSection()->parameter) {
        writePair(output, iterator);
    }

    for (const auto &[fst, snd]: config_file->getAllSection()) {
        output->write("[", 1);
        output->write(fst.c_str(), fst.length());
        output->write("]\n", 2);
        for (const auto &iterator: snd->parameter) {
            writePair(output, iterator);
        }
    }

    return true;
}

std::string ConfigUtil::readLine(io::InputStream *input) {
    size_t count = 0;
    char ch;
    text::StringBuilder stream;
    while (true) {
        if (const auto SUCCESS = input->read(&ch, 1); SUCCESS < 1) {
            return stream.toString();
        }
        if (ch == '\r') {
        } else if (ch != '\n') {
            stream << ch;
            count += 1;
        } else {
            if (count > 0) {
                return stream.toString();
            }
        }
    }
}

} // namespace sese