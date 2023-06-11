/**
 * \file InputBufferWrapper.h
 * \author kaoru
 * \date 2022.12.14
 * \version 0.1
 * \brief 输入缓存包装器
 */
#pragma once

#include "sese/util/InputStream.h"
#include "sese/util/PeekableStream.h"

namespace sese {

    /// \brief 输入缓存包装器
    class API InputBufferWrapper : public InputStream, public PeekableStream {
    public:
        InputBufferWrapper(const char *buffer, size_t cap);

        int64_t read(void *buffer, size_t length) override;

        int64_t peek(void *buffer, size_t length) override;

        int64_t trunc(size_t length) override;

        void reset() noexcept;

        [[nodiscard]] const char *getBuffer() const;
        [[nodiscard]] size_t getLen() const;
        [[nodiscard]] size_t getCap() const;

    protected:
        const char *buffer = nullptr;
        size_t pos = 0;
        size_t cap = 0;
    };

}