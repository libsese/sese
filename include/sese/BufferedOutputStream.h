#pragma once
#include <sese/Stream.h>
#include <sese/Noncopyable.h>

namespace sese {

    class API BufferedOutputStream : public Noncopyable {
    public:
        explicit BufferedOutputStream(const Stream::Ptr &source, size_t bufferSize = STREAM_BYTE_STREAM_SIZE_FACTOR);
        ~BufferedOutputStream() noexcept;

        int64_t write(const void *buffer, size_t length) noexcept;

        int64_t flush();

    private:
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