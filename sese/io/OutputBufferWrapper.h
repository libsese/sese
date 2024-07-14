/**
* \file OutputBufferWrapper.h
* \author kaoru
* \date 2022.12.14
* \version 0.1
* \brief 输出缓存包装器
*/

#pragma once

#include "sese/io/OutputStream.h"

#include <cstdint>

namespace sese::io {

// GCOVR_EXCL_START

/// \brief 输出缓存包装器
class  OutputBufferWrapper final : public OutputStream {
public:
    OutputBufferWrapper(char *buffer, size_t cap);

    int64_t write(const void *buffer, size_t length) override;

    void reset() noexcept;

    [[nodiscard]] const char *getBuffer() const;
    [[nodiscard]] size_t getLength() const;
    [[nodiscard]] size_t getCapacity() const;

protected:
    char *buffer = nullptr;
    size_t len = 0;
    size_t cap = 0;
};

// GCOVR_EXCL_STOP

} // namespace sese::io