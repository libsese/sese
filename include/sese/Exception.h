/**
 * @file Exception.h
 * @brief 异常基类
 * @author kaoru
 * @date 2022年3月28日
 */
#pragma once
#include <exception>
#include "Config.h"

#ifdef _WIN32
#pragma warning (disable : 4275)
#pragma warning (disable : 4251)
#endif

namespace sese {
    /**
     * @brief 异常基类
     */
    class API Exception : std::exception {
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