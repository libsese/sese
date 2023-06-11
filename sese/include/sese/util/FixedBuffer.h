/// \file FixedBuffer.h
/// \brief 线程安全的固定大小缓存
/// \author kaoru
/// \version 0.1.0
/// \date 2023年6月11日

#pragma once

#include "sese/util/AbstractFixedBuffer.h"

#include <mutex>

namespace sese {

    /// 线程安全的固定大小缓存
    class API FixedBuffer final : public AbstractFixedBuffer {
    public:
        explicit FixedBuffer(size_t size) noexcept;

        int64_t read(void *buffer, size_t length) override;

        int64_t write(const void *buffer, size_t length) override;

        int64_t peek(void *buffer, size_t length) override;

        int64_t trunc(size_t length) override;

        void reset() noexcept override;

        size_t getReadableSize() noexcept;

        [[nodiscard]] size_t getWriteableSize() noexcept;

    private:
        std::mutex mutex;
    };

}// namespace sese