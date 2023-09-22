#pragma once

#include "sese/native/Windows/Event/WSAEvent.h"
#include "sese/native/Windows/event/WSAEventConvert.h"
#include "sese/event/BaseEventLoop.h"

#include <atomic>
#define MAX_EVENT_SIZE 64

namespace sese::event {

    class WSAEventLoop : public BaseEventLoop {
    public:
        ~WSAEventLoop() override;

        bool init() override;

        void dispatch(uint32_t timeout) override;

        void onAccept(int fd) override;

        void onRead(BaseEvent *event) override;

        void onWrite(BaseEvent *event) override;

        void onError(BaseEvent *event) override;

        void onClose(BaseEvent *event) override;

        BaseEvent *createEvent(int fd, unsigned int events, void *data) override;

        void freeEvent(BaseEvent *event) override;

        bool setEvent(BaseEvent *event) override;

        void setListenFd(int fd) override;

    protected:
        int listenFd{-1};
        WSAEvent *listenEvent{nullptr};

        void *wsaEvent{nullptr};
        WSAEventConvert convert;

        unsigned long numbers = 0;
        unsigned long long sockets[MAX_EVENT_SIZE]{};
        void *wsaEvents[MAX_EVENT_SIZE]{};
        WSAEvent *events[MAX_EVENT_SIZE]{};// 此处生命周期应由用户负责
    };

}// namespace sese::event

#undef MAX_EVENT_SIZE