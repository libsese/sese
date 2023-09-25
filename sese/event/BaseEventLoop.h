#pragma once

#include <sese/event/BaseEvent.h>

#include <cstdint>

namespace sese::event {

    class BaseEventLoop {
    public:
        virtual bool init() = 0;

        virtual ~BaseEventLoop() = default;

        virtual void dispatch(uint32_t timeout) = 0;

        virtual void onAccept(int fd) = 0;

        virtual void onRead(BaseEvent *event) = 0;

        virtual void onWrite(BaseEvent *event) = 0;

        virtual void onError(BaseEvent *event) = 0;

        virtual void onClose(BaseEvent *event) = 0;

        virtual BaseEvent *createEvent(int fd, unsigned int events, void *data) = 0;

        virtual void freeEvent(BaseEvent *event) = 0;

        virtual bool setEvent(BaseEvent *event) = 0;

        virtual void setListenFd(int fd) = 0;
    };

}// namespace sese::event