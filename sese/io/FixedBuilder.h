// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/// \file FixedBuilder.h
/// \author kaoru
/// \brief 线程不安全的固定大小缓存
/// \date 2023年6月11日
/// \version 0.1.0

#pragma once

#include "sese/io/AbstractFixedBuffer.h"

namespace sese::io {

/// 线程不安全的固定大小缓存
class  FixedBuilder final : public AbstractFixedBuffer {
public:
    explicit FixedBuilder(size_t size) noexcept : AbstractFixedBuffer(size) {}
};

} // namespace sese::io