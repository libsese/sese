/// \file BufferedOutputStream.h
/// \brief 输出流缓存类
/// \version 0.1
/// \author kaoru
/// \date 2022.11.20
#pragma once
#include "OutputStream.h"

namespace sese {

    /// 输出流缓存类
    class API BufferedOutputStream : public OutputStream {
    public:
        using Ptr = std::shared_ptr<BufferedOutputStream>;

        explicit BufferedOutputStream(const OutputStream::Ptr &source, size_t bufferSize = STREAM_BYTE_STREAM_SIZE_FACTOR);

        ~BufferedOutputStream() noexcept;

        int64_t write(const void *buffer, size_t length) override;

        int64_t flush() noexcept;

        [[nodiscard]] size_t getPos() const { return pos; }
        [[nodiscard]] size_t getLen() const { return len; }
        [[nodiscard]] size_t getCap() const { return cap; }

    private:
        OutputStream::Ptr source;
        void *buffer = nullptr;
        size_t pos;
        size_t len;
        size_t cap;
    };
}