/**
 * @file StringBuilder.h
 * @author kaoru
 * @date 2022年3月28日
 * @brief 线程不安全的字符串缓存类
 */
#pragma once
#include "sese/text/AbstractStringBuffer.h"
#include "sese/Config.h"

#ifdef _WIN32
#pragma warning(disable : 4275)
#endif

namespace sese::text {

    /**
     * @brief 线程不安全的字符串缓存类
     */
    class API StringBuilder final : public AbstractStringBuffer {
    public:
        using Ptr = std::unique_ptr<StringBuilder>;

        explicit StringBuilder(size_t cap = STRING_BUFFER_SIZE_FACTOR) noexcept : AbstractStringBuffer(cap) {
        }

        explicit StringBuilder(const char *str) noexcept : AbstractStringBuffer(str) {
        }
    };
}// namespace sese