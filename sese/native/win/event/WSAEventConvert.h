/**
 * @file WSAEventConvert.h
 * @brief WSAEventSelect 事件转换器
 * @author kaoru
 */

#pragma once

#include "sese/event/BaseEventConvert.h"

namespace sese::event {
/// WSAEventSelect 事件转换器
class WSAEventConvert : public BaseEventConvert {
public:
    unsigned int fromNativeEvent(int event) override;

    int toNativeEvent(unsigned int event) override;
};
} // namespace sese::event