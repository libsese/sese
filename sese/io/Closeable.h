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
 * @file Closeable.h
 * @date 2023年7月5日
 * @author kaoru
 * @brief 可关闭接口类
 * @version 0.1
 */

#pragma once

namespace sese::io {
/// 可关闭接口
class  Closeable {
public:
    virtual ~Closeable() = default;
    virtual void close() = 0;
};
} // namespace sese::io