#include "sese/native/Linux/event/EpollEventConvert.h"

#include <sys/epoll.h>

unsigned int sese::event::EpollEventConvert::fromNativeEvent(int event) {
    short result = 0;
    // if (event & EPOLLIN) {
    //     result |= EVENT_READ;
    // }
    if (event & EPOLLOUT) {
        result |= EVENT_WRITE;
    }
    if (event & EPOLLERR) {
        result |= EVENT_ERROR;
    }
    return result;
}

int sese::event::EpollEventConvert::toNativeEvent(unsigned int event) {
    int result = 0;
    if (event & EVENT_READ) {
        result |= EPOLLIN;
    }
    if (event & EVENT_WRITE) {
        result |= EPOLLOUT;
    }
    if (event & EVENT_ERROR) {
        result |= EPOLLERR;
    }
    return result;
}
