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

#include <sese/config/ini/IniUtil.h>
#include <sese/io/InputStreamReader.h>

using namespace sese::ini;

IniUtil::IniConfigObject IniUtil::parse(IniUtil::InputStream *input) noexcept {
    auto reader = io::InputStreamReader(input);

    auto config = std::make_unique<IniConfig>();
    auto current = &config->defSection;

    while (true) {
        std::string line = reader.readLine();
        if (line.empty()) {
            break;
        }

        if (line[0] == '[') {
            // new section
            auto pos = line.find_last_of(']');
            if (pos == std::string::npos) {
                continue;
            }
            auto iterator = config->sectionMap.emplace(line.substr(1, pos - 1), std::map<std::string, std::string>{});
            current = &iterator.first->second;
        } else if (line[0] == '#') {
            // comment
            continue;
        } else {
            // current section
            auto pos = line.find_first_of('=');
            if (pos == std::string::npos) {
                continue;
            }
            current->emplace(line.substr(0, pos), line.substr(pos + 1));
        }
    }

    return config;
}

#define ASSERT_WRITE(buf, size) \
    if (output->write(buf, size) != size) return false;

// The branch here is basically used to determine whether the write is successful
// GCOVR_EXCL_START

bool IniUtil::unparse(const IniUtil::IniConfigObject &target, IniUtil::OutputStream *output) noexcept {
    for (auto &&item: target->defSection) {
        if (!unparsePair(item, output)) {
            return false;
        }
    }
    ASSERT_WRITE("\n", 1)

    for (auto &&map: target->sectionMap) {
        ASSERT_WRITE("[", 1)
        ASSERT_WRITE(map.first.c_str(), map.first.length())
        ASSERT_WRITE("]\n", 2)
        for (auto &&item: map.second) {
            if (!unparsePair(item, output)) {
                return false;
            }
        }
        ASSERT_WRITE("\n", 1)
    }

    return true;
}

bool IniUtil::unparsePair(const std::pair<std::string, std::string> &pair, OutputStream *output) noexcept {
    ASSERT_WRITE(pair.first.c_str(), pair.first.length())
    ASSERT_WRITE("=", 1)
    ASSERT_WRITE(pair.second.c_str(), pair.second.length())
    ASSERT_WRITE("\n", 1)
    return true;
}

// GCOVR_EXCL_STOP
