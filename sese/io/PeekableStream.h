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

/// \file PeekableStream.h
/// \author kaoru
/// \brief 可窥视流接口
/// \version 0.2
/// \date 2023年6月11日

#pragma once

#include "sese/Config.h"

namespace sese::io {

// GCOVR_EXCL_START

/// 可窥视流接口
class PeekableStream {
public:
    virtual ~PeekableStream() = default;
    typedef std::shared_ptr<PeekableStream> Ptr;

    /// 仅读取内容，不步进
    /// \param buffer 缓存
    /// \param length 缓存大小
    /// \return 实际读取大小
    virtual int64_t peek(void *buffer, size_t length) = 0;

    /// 仅步进，不读取内容
    /// \param length 步进大小
    /// \return 实际步进大小
    virtual int64_t trunc(size_t length) = 0;
};

// GCOVR_EXCL_STOP

} // namespace sese::io