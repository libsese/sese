/**
 * \file OutputStream.h
 * \date 2022年11月14日
 * \author kaoru
 * \brief 流式输出接口类
 * \version 0.1
 */
#pragma once

#include "sese/Config.h"
#include "sese/text/String.h"

#include <span>

namespace sese::io {

// GCOVR_EXCL_START

/// \brief 流式输出接口类
/// \warning 支持写入 std::vector、std::array、std::span 等类型，但需要注意 T 是否为 POD 类型，非 POD 类型写入将会导致不可预期事件
class  OutputStream {
public:
    typedef std::shared_ptr<OutputStream> Ptr;

    virtual ~OutputStream() noexcept = default;

    virtual int64_t write(const void *buffer, size_t length) = 0;

    int64_t write(const std::string_view &buffer);

    int64_t write(const text::StringView &buffer);

    template<typename T>
    int64_t write(std::vector<T> &buffer) {
        return write(buffer.data(), buffer.size());
    }

    template<typename T, size_t N>
    int64_t write(std::array<T, N> &buffer) {
        return write(buffer.data(), N * sizeof(T));
    }

    template<typename T>
    int64_t write(std::span<T> &buffer) {
        return write(buffer.data(), buffer.size() * sizeof(T));
    }
};

// GCOVR_EXCL_STOP

} // namespace sese::io