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

#include <sese/container/StrCaseMap.h>
#include <sese/log/Marco.h>
#include <sese/Util.h>

#include <algorithm>


bool sese::StrCaseMapComparer::operator()(const std::string &lv, const std::string &rv) const {
    return sese::StrCmpI()(lv, rv) < 0;
}

size_t sese::StrCaseUnorderedMapComparer::operator()(const std::string &key) const {
#if defined(SESE_PLATFORM_WINDOWS) && !defined(__MINGW32__)
#define XX std::tolower
#else
#define XX ::tolower
#endif
    auto temp = key;
    std::transform(temp.begin(), temp.end(), temp.begin(), tolower);
    return std::hash<std::string>{}(temp);
#undef XX
}

bool sese::StrCaseUnorderedMapComparer::operator()(const std::string &lv, const std::string &rv) const {
    return sese::StrCmpI()(lv, rv) == 0;
}