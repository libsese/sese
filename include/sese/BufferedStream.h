#pragma once
#include <sese/Stream.h>
#include <sese/Noncopyable.h>

namespace sese {

    class API BufferedStream : public Noncopyable, public Stream {
    public:
        explicit BufferedStream(const Stream::Ptr &source, size_t bufferSize = STREAM_BYTE_STREAM_SIZE_FACTOR);
        ~BufferedStream() noexcept;

        int64_t read(void *buffer, size_t length) override;

        void clear() noexcept;

        int64_t write(const void *buffer, size_t length) override;

        int64_t flush() noexcept;

        void close() override;

        [[nodiscard]] size_t getPos() const { return pos; }
        [[nodiscard]] size_t getLen() const { return len; }
        [[nodiscard]] size_t getCap() const { return cap; }

    private:
        int64_t preRead();

        /// 源流指针
        Stream::Ptr source;
        /// 缓存指针
        void *buffer = nullptr;
        /// 已操作的数据大小
        size_t pos;
        /// 可供操作的数据大小
        size_t len;
        /// 缓存容量
        size_t cap;
    };
}// namespace sese