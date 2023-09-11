#pragma once

#include "sese/Config.h"
#include "sese/io/Closeable.h"
#include "sese/io/Stream.h"

namespace sese::io {

    // 此处已经过单测，不存在分支和未测函数
    // GCOVR_EXCL_START

    template<typename T>
    class FakeStream : public Stream {
    public:
        explicit FakeStream(T *t) : t(t) {}

        int64_t read(void *buffer, size_t length) override { return t->read(buffer, length); }
        int64_t write(const void *buffer, size_t length) override { return t->write(buffer, length); };

    protected:
        T *t;
    };

    template<typename T>
    class ClosableFakeStream : public FakeStream<T>, public sese::io::Closeable {
    public:
        explicit ClosableFakeStream(T *t) : FakeStream<T>(t) {}
        void close() override { this->t->close(); }
    };

    // GCOVR_EXCL_STOP

}// namespace sese
