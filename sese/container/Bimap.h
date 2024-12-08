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

/// \file Bimap.h
/// \brief Bidirectional map
/// \author kaoru
/// \date January 24, 2024

#pragma once

#include <cstddef>
#include <map>
#include <initializer_list>

namespace sese {

/// \brief Bidirectional map
/// \tparam K Key type
/// \tparam V Value type
template<class K, class V>
class Bimap final {
public:
    using BimapKeyType = K;
    using BimapValueType = V;

    Bimap() = default;
    ~Bimap() = default;
    Bimap(const Bimap &) = default;
    Bimap &operator=(const Bimap &) = default;
    Bimap(Bimap &&)  noexcept = default;
    Bimap &operator=(Bimap &&)  noexcept = default;

    Bimap(const std::initializer_list<std::pair<BimapKeyType, BimapValueType>> &init) {
        for (const auto &[key, value]: init) {
            insert(key, value);
        }
    }

    decltype(auto) begin() const { return first.begin(); }

    decltype(auto) end() const { return first.end(); }

    void insert(const BimapKeyType &key, const BimapValueType &value) {
        first[key] = value;
        second[value] = key;
    }

    void eraseByKey(const BimapKeyType &key) {
        BimapValueType value = first[key];
        first.erase(key);
        second.erase(value);
    }

    void eraseByValue(const BimapValueType &value) {
        BimapKeyType key = second[value];
        first.erase(key);
        second.erase(value);
    }

    bool tryEraseByKey(const BimapKeyType &key) {
        if (!first.contains(key)) return false;
        eraseByKey(key);
        return true;
    }

    bool tryEraseByValue(const BimapValueType &value) {
        if (!second.contains(value)) return false;
        eraseByValue(value);
        return true;
    }

    bool existByKey(const BimapKeyType &key) const {
        return first.contains(key);
    }

    bool existByValue(const BimapValueType &value) const {
        return second.contains(value);
    }

    [[nodiscard]] const BimapValueType &getByKey(const BimapKeyType &key) const {
        return first.at(key);
    }

    [[nodiscard]] BimapValueType &getByKey(const BimapKeyType &key) {
        return first.at(key);
    }

    [[nodiscard]] const BimapKeyType &getByValue(const BimapValueType &value) const {
        return second.at(value);
    }

    [[nodiscard]] BimapKeyType &getByValue(const BimapValueType &value) {
        return second.at(value);
    }

    [[nodiscard]] bool empty() const {
        return first.empty();
    }

    [[nodiscard]] size_t size() const {
        return first.size();
    }

    void clear() {
        first.clear();
        second.clear();
    }

private:
    std::map<BimapKeyType, BimapValueType> first{};
    std::map<BimapValueType, BimapKeyType> second{};
};
} // namespace sese