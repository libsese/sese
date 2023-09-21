#pragma once

#include "sese/event/BaseEvent.h"
#include "sese/event/EpollEvent.h"
#include "sese/event/BaseEventLoop.h"
#include "sese/event/EpollEventConvert.h"

namespace sese {
    namespace event {
        class EpollEventLoop;
    }
}

class sese::event::EpollEventLoop : public sese::event::BaseEventLoop {
public:
    bool init() override;

    ~EpollEventLoop() override;

    void dispatch(uint32_t timeout) override;

    void onAccept(int fd) override;

    void onRead(BaseEvent *event) override;

    void onWrite(BaseEvent *event) override;

    void onError(BaseEvent *event) override;

    void onClose(BaseEvent *event) override;

    BaseEvent *createEvent(int fd, unsigned int events, void *data) override;

    void freeEvent(BaseEvent *event) override;

    bool setEvent(BaseEvent *event) override;

    void setListenFd(int fd) override { listenFd = fd; }

protected:
    int listenFd{-1};
    BaseEvent *listenEvent{nullptr};

    int epoll{-1};
    EpollEventConvert convert;
};