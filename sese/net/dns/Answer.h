/// \file Answer.h
/// \author kaoru
/// \brief DNS 应答类
/// \version 0.1
/// \date 2023年8月13日

#pragma once

#include <sese/Config.h>

namespace sese::net::dns {

/// DNS 应答类
class API Answer {
public:
    Answer(std::string name, uint16_t type, uint16_t class_, uint32_t time, uint16_t len, std::string data) noexcept
        : name(std::move(name)), type(type), class_(class_), liveTime(time), length(len), data(std::move(data)) {}

    [[nodiscard]] const std::string &getName() const { return name; }

    [[nodiscard]] uint16_t getType() const { return type; }

    [[nodiscard]] uint16_t getClass() const { return class_; }

    [[nodiscard]] uint32_t getLiveTime() const { return liveTime; }

    [[nodiscard]] uint16_t getLength() const { return length; }

    [[nodiscard]] const std::string &getData() const { return data; }

private:
    std::string name;
    uint16_t type;
    uint16_t class_;
    uint32_t liveTime;
    uint16_t length;
    std::string data;
};

} // namespace sese::net::dns