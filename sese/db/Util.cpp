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

#include <sese/db/Util.h>
#include <sese/text/StringBuilder.h>

using sese::text::StringBuilder;

std::map<std::string, std::string> sese::db::tokenize(const char *p) noexcept {
    std::map<std::string, std::string> result;

    auto str = StringBuilder::split(p, ";");
    for (auto &&item: str) {
        auto pair = StringBuilder::split(item, "=");
        auto &&key = pair[0];
        auto &&value = pair[1];
        result[key] = value;
    }

    return result;
}