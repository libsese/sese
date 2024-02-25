/// \file Bimap.h
/// \brief 双向映射表
/// \author kaoru
/// \date 2024年01月24日

#pragma once

#include <cstddef>
#include <map>
#include <initializer_list>

namespace sese {

/// 双向映射表
/// \tparam K 键类型
/// \tparam V 值类型
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