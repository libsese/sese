#include "sese/native/win/event/WSAEventConvert.h"

#include <winsock2.h>

unsigned int sese::event::WSAEventConvert::fromNativeEvent(int event) {
    unsigned result = 0;
    if (event & FD_READ) {
        result |= EVENT_READ;
    }
    if (event & FD_WRITE) {
        result |= EVENT_WRITE;
    }
    return result;
}

int sese::event::WSAEventConvert::toNativeEvent(unsigned int event) {
    int result = 0;
    if (event & EVENT_READ) {
        result |= FD_READ;
    }
    if (event & EVENT_WRITE) {
        result |= FD_WRITE;
    }
    return result;
}
