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

/// \file StrCaseMap.h
/// \author kaoru
/// \date May 9, 2024
/// \brief Dictionary class that ignores case of string keys

#pragma once

#include <map>
#include <unordered_map>
#include <string>

namespace sese {

/// \brief StrCaseMap Comparator
struct StrCaseMapComparer {
    bool operator()(const std::string &lv, const std::string &rv) const;
};

/// \brief StrCaseUnorderedMap Comparator
struct StrCaseUnorderedMapComparer {
    size_t operator()(const std::string &key) const;
    bool operator()(const std::string &lv, const std::string &rv) const;
};

/// \brief Dictionary class that ignores case of string keys
/// \tparam VALUE Dictionary value type
/// \tparam ALLOCATOR Memory allocator
template<typename VALUE, typename ALLOCATOR = std::allocator<std::pair<const std::string, VALUE>>>
using StrCaseMap = std::map<std::string, VALUE, StrCaseMapComparer, ALLOCATOR>;

/// \brief Dictionary class that ignores case of string keys
/// \tparam VALUE Dictionary value type
/// \tparam ALLOCATOR Memory allocator
template<typename VALUE, typename ALLOCATOR = std::allocator<std::pair<const std::string, VALUE>>>
using StrCaseUnorderedMap = std::unordered_map<std::string, VALUE, StrCaseUnorderedMapComparer, StrCaseUnorderedMapComparer, ALLOCATOR>;

} // namespace sese