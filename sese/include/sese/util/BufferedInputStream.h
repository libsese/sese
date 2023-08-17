/// \file BufferedInputStream.h
/// \brief 输入流缓存类
/// \version 0.1
/// \author kaoru
/// \date 2022.11.20
#pragma once
#include "InputStream.h"

#ifdef WIN32
#pragma warning(disable : 4251)
#endif

namespace sese {

    /// 输入流缓存类
    class API BufferedInputStream : public InputStream {
    public:
        using Ptr = std::shared_ptr<BufferedInputStream>;

        explicit BufferedInputStream(const InputStream::Ptr &source, size_t bufferSize = STREAM_BYTE_STREAM_SIZE_FACTOR);

        ~BufferedInputStream() noexcept;

        int64_t read(void *buffer, size_t length) override;

        [[nodiscard]] size_t getPosition() const { return pos; }
        [[nodiscard]] size_t getLength() const { return len; }
        [[nodiscard]] size_t getCapacity() const { return cap; }

    private:
        int64_t preRead() noexcept;

    private:
        InputStream::Ptr source;
        void *buffer = nullptr;
        size_t pos;
        size_t len;
        size_t cap;
    };
}// namespace sese