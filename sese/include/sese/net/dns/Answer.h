#pragma once

#include <sese/Config.h>

namespace sese::net::dns {

    class API Answer {
    public:
        Answer(bool ref, std::string name, uint16_t type, uint16_t class_, uint32_t time, uint16_t len, std::string addr) noexcept
            : ref(ref), name(std::move(name)), type(type), class_(class_), liveTime(time), length(len), addr(std::move(addr)) {}

        [[nodiscard]] const std::string &getName() const { return name; }

        [[nodiscard]] uint16_t getType() const { return type; }

        [[nodiscard]] uint16_t getClass() const { return class_; }

        [[nodiscard]] uint32_t getLiveTime() const { return liveTime; }

        [[nodiscard]] uint16_t getLength() const { return length; }

        [[nodiscard]] const std::string &getAddress() const { return addr; }

        [[nodiscard]] bool isRef() const { return ref; }

    private:
        bool ref = false;
        std::string name;
        uint16_t type;
        uint16_t class_;
        uint32_t liveTime;
        uint16_t length;
        std::string addr;
    };

}// namespace sese::net::dns