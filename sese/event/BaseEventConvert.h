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
}// namespace sese::event