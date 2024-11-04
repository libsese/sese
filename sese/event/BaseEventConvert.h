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
 * @file BaseEventConvert.h
 * @brief 基础网络事件转换器接口
 * @author kaoru
 */

#pragma once

#include "sese/event/BaseEvent.h"

namespace sese::event {
/// 基础网络事件转换器接口
class BaseEventConvert {
public:
    virtual ~BaseEventConvert() = default;

    virtual unsigned int fromNativeEvent(int event) = 0;

    virtual int toNativeEvent(unsigned int event) = 0;
};
} // namespace sese::event