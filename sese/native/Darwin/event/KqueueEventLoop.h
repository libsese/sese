#pragma once

#include <sese/event/BaseEventLoop.h>
#include <sese/native/Darwin/event/KqueueEvent.h>

#include <atomic>

namespace sese::event {

    class KqueueEventLoop : public BaseEventLoop {
    public:
        bool init() override;

        ~KqueueEventLoop() override;

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

        //protected:
        //    bool addNativeEvent(int fd, uint32_t ev, void *data) const;
        //
        //    bool delNativeEvent(int fd, uint32_t ev, void *data) const;

    protected:
        int listenFd{-1};
        BaseEvent *listenEvent{nullptr};
        int kqueue{-1};
    };

}// namespace sese::event