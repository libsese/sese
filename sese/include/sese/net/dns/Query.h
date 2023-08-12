#pragma once

#include <sese/Config.h>

namespace sese::net::dns {

    class API Query {
    public:
        Query(std::string name, uint16_t type, uint16_t class_, uint32_t len) noexcept
            : name(std::move(name)), type(type), class_(class_), length(len) {}

        [[nodiscard]] const std::string &getName() const { return name; }

        [[nodiscard]] uint16_t getType() const { return type; }

        [[nodiscard]] uint16_t getClass() const { return class_; }

    private:
        std::string name;
        uint16_t type{};
        uint16_t class_{};
        uint32_t length{};
    };

}// namespace sese::net::dns