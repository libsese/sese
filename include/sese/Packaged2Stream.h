#pragma once
#include <sese/Config.h>
#include <sese/Stream.h>

namespace sese {

    template<typename T>
    class PackagedStream : public Stream {
    public:
        explicit PackagedStream(T *t) : t(t) {}

        int64_t read(void *buffer, size_t length) override { return t->read(buffer, length); }
        int64_t write(const void *buffer, size_t length) override { return t->write(buffer, length); };
        void close() override{};

    protected:
        T *t;
    };

    template<typename T>
    class ClosablePackagedStream : public PackagedStream<T> {
    public:
        explicit ClosablePackagedStream(T *t) : PackagedStream<T>(t) {}
        void close() override { this->t->close(); }
    };

}// namespace sese
