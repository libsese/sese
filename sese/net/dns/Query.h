/// \file Query.h
/// \author kaoru
/// \brief DNS 查询类
/// \version 0.1
/// \date 2023年8月13日

#pragma once

#include <sese/Config.h>

namespace sese::net::dns {

/// DNS 查询类
class  Query {
public:
    Query(std::string name, uint16_t type, uint16_t class_, uint16_t offset) noexcept
        : name(std::move(name)), type(type), class_(class_), offset(offset) {}

    [[nodiscard]] const std::string &getName() const { return name; }

    [[nodiscard]] uint16_t getType() const { return type; }

    [[nodiscard]] uint16_t getClass() const { return class_; }

    [[nodiscard]] uint16_t getOffset() const { return offset; }

private:
    std::string name;
    uint16_t type{};
    uint16_t class_{};
    uint16_t offset{};
};

} // namespace sese::net::dns