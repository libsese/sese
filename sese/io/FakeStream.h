/**
 * @file FakeStream.h
 * @author kaoru
 * @brief 伪装流模板
 * @version 0.1
 * @date 2023年9月13日
 */

#pragma once

#include "sese/Config.h"
#include "sese/io/Closeable.h"
#include "sese/io/Stream.h"

namespace sese::io {

    // 此处已经过单测，不存在分支和未测函数
    // GCOVR_EXCL_START

    /// \brief 伪装流
    /// \tparam T 包装类型
    template<typename T>
    class FakeStream : public Stream {
    public:
        explicit FakeStream(T *t) : t(t) {}

        int64_t read(void *buffer, size_t length) override { return t->read(buffer, length); }
        int64_t write(const void *buffer, size_t length) override { return t->write(buffer, length); };

    protected:
        T *t;
    };

    /// \brief 可关闭的伪装流
    /// \tparam T 包装类型
    template<typename T>
    class ClosableFakeStream : public FakeStream<T>, public sese::io::Closeable {
    public:
        explicit ClosableFakeStream(T *t) : FakeStream<T>(t) {}
        void close() override { this->t->close(); }
    };

    // GCOVR_EXCL_STOP

}// namespace sese
