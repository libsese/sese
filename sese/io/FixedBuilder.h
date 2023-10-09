/// \file FixedBuilder.h
/// \author kaoru
/// \brief 线程不安全的固定大小缓存
/// \date 2023年6月11日
/// \version 0.1.0

#pragma once

#include "sese/io/AbstractFixedBuffer.h"

namespace sese::io {

/// 线程不安全的固定大小缓存
class API FixedBuilder final : public AbstractFixedBuffer {
public:
    explicit FixedBuilder(size_t size) noexcept : AbstractFixedBuffer(size) {}
};

} // namespace sese::io