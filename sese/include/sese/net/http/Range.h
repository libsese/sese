#pragma once

#include <sese/Config.h>

#include <vector>

namespace sese::net::http {

    struct Range {
        size_t begin = 0;
        size_t len = 0;
        size_t cur = 0;

        Range(size_t begin, size_t len) noexcept;

        /// @brief 解析 Range 字段
        /// @param str Range 值
        /// @example bytes=0-1023
        /// @example bytes=0-50, 100-150, 200-
        /// @return Range
        static std::vector<Range> parse(const std::string &str, size_t total) noexcept;

        [[nodiscard]] std::string toString(size_t total) const noexcept;
    };

}// namespace sese::net::http