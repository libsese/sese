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
 * \file UuidBuilder.h
 * \author kaoru
 * \date 2022.12.24
 * \version 0.1
 * \brief UUID 生成相关
 */

#pragma once

#ifdef WIN32
#pragma warning(disable : 4251)
#endif

#include "sese/Config.h"
#include "sese/util/TimestampHandler.h"
#include "sese/util/Uuid.h"

namespace sese {
/// Uuid 生成器
class UuidBuilder {
public:
    /// 构造函数
    explicit UuidBuilder() noexcept;

    /// 生成 UUID
    /// \param dest 存放结果的 UUID
    /// \return 是否生成成功
    bool generate(sese::Uuid &dest) noexcept;

protected:
    sese::TimestampHandler timestampHandler;
};
} // namespace sese
