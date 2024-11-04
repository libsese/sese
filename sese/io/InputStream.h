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

/**
 * \file InputStream.h
 * \date 2022年11月14日
 * \author kaoru
 * \brief 流式输入接口类
 * \version 0.1
 */
#pragma once

#include <memory>

// GCOVR_EXCL_START

namespace sese::io {

/// \brief 流式输入接口类
class  InputStream {
public:
    typedef std::shared_ptr<InputStream> Ptr;

    virtual ~InputStream() noexcept = default;

    virtual int64_t read(void *buffer, size_t length) = 0;
};

} // namespace sese::io

// GCOVR_EXCL_STOP