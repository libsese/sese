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