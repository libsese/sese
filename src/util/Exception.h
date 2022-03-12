#pragma once
#include <exception>
#include <string>
#include "Config.h"

namespace sese {
    class Exception : std::exception {
    public:
        Exception() { message = EXCEPTION_STRING; }

        explicit Exception(const std::string &str) {
            message = EXCEPTION_HEADER_STRING + str;
        }

        [[nodiscard]] const char *what() const noexcept override {
            return message.c_str();
        }

    protected:
        std::string message;
    };
}// namespace sese