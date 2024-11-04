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
*  @file EpollEventConvert.h
*  @brief epoll 事件转换器
*  @author kaoru
*/

#pragma once

#include "sese/event/BaseEventConvert.h"

namespace sese::event {
/// epoll 事件转换器
class EpollEventConvert : public BaseEventConvert {
public:
    unsigned int fromNativeEvent(int event) override;

    int toNativeEvent(unsigned int event) override;
};
} // namespace sese::event