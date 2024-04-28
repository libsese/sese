/**
* @file Range.h
* @author kaoru
* @version 0.1
* @brief HTTP 内容区间类
* @date 2023年9月13日
*/

#pragma once

#include <sese/Config.h>

#include <vector>

namespace sese::net::http {

/// HTTP 内容区间类
struct Range {
    size_t begin = 0;
    size_t len = 0;
    size_t cur = 0;

    Range(size_t begin, size_t len) noexcept;

    /** @brief 解析 Range 字段
     *  @param str Range 值
     *  @verbatim
        bytes=0-1023
        bytes=0-50, 100-150, 200-
     *  @endverbatim
     *  @param total 区间总大小
     *  @return Range
     **/
    static std::vector<Range> parse(const std::string &str, size_t total) noexcept;

    [[nodiscard]] std::string toString(size_t total) const noexcept;

    [[nodiscard]] size_t toStringLength(size_t total) const noexcept;
};

} // namespace sese::net::http