#include "sese/event/BaseEvent.h"
#include "sese/native/Linux/event/EpollEventLoop.h"
#include "sese/native/Linux/event/EpollEvent.h"

#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>

#define MAX_EVENT_SIZE 64

bool sese::event::EpollEventLoop::init() {
    epoll = epoll_create1(0);
    if (-1 == epoll) return false;
    if (0 >= listenFd) return true;

    this->listenEvent = this->createEvent(listenFd, EVENT_READ | EVENT_ERROR, nullptr);
    // 通常不会失败
    if (!this->listenEvent) return false;
    return true;
}

sese::event::EpollEventLoop::~EpollEventLoop() {
    if (-1 != epoll) {
        close(epoll);
        epoll = -1;
    }

    if (listenEvent) {
        delete listenEvent;
        listenEvent = nullptr;
    }
}

void sese::event::EpollEventLoop::dispatch(uint32_t timeout) {
    epoll_event events[MAX_EVENT_SIZE]{};

    int numberOfFds = epoll_wait(epoll, events, MAX_EVENT_SIZE, (int) timeout);
    if (-1 == numberOfFds) {
        return;
    }

    for (int i = 0; i < numberOfFds; ++i) {
        auto event = (BaseEvent *) events[i].data.ptr;
        auto fd = event->fd;
        if (fd == listenFd) {
            auto client = accept(fd, nullptr, nullptr);
            if (-1 != client) {
                onAccept(client);
            } else {
                continue;
            }
        } else {
            if (events[i].events & EPOLLIN) {
                if (event->events & EVENT_READ) {
                    onRead((BaseEvent *) events[i].data.ptr);
                }
                if (events[i].events & EPOLLRDHUP) {
                    onClose((BaseEvent *) events[i].data.ptr);
                    continue;
                }
            }
            if (events[i].events & EPOLLOUT) {
                if (event->events & EVENT_WRITE) {
                    onWrite((BaseEvent *) events[i].data.ptr);
                }
                if (events[i].events & EPOLLRDHUP) {
                    onClose((BaseEvent *) events[i].data.ptr);
                    continue;
                }
            }
            if (events[i].events & EPOLLERR) {
                onError((BaseEvent *) events[i].data.ptr);
            }
        }
    }
}

void sese::event::EpollEventLoop::onAccept(int fd) {
}

void sese::event::EpollEventLoop::onRead(BaseEvent *event) {
}

void sese::event::EpollEventLoop::onWrite(BaseEvent *event) {
}

void sese::event::EpollEventLoop::onError(BaseEvent *event) {
}

void sese::event::EpollEventLoop::onClose(sese::event::BaseEvent *event) {
}

sese::event::BaseEvent *sese::event::EpollEventLoop::createEvent(int fd, unsigned int events, void *data) {
    auto event = new EpollEvent;
    event->fd = fd;
    event->events = events;
    event->data = data;

    epoll_event epollEvent{};
    epollEvent.events = convert.toNativeEvent(events) | EPOLLRDHUP | EPOLLIN | EPOLLET;
    epollEvent.data.ptr = event;
    if (-1 == epoll_ctl(epoll, EPOLL_CTL_ADD, fd, &epollEvent)) {
        delete event;
        return nullptr;
    }

    return event;
}

void sese::event::EpollEventLoop::freeEvent(sese::event::BaseEvent *event) {
    epoll_ctl(epoll, EPOLL_CTL_DEL, event->fd, nullptr);
    delete event;
}

bool sese::event::EpollEventLoop::setEvent(sese::event::BaseEvent *event) {
    epoll_event epollEvent{};
    epollEvent.events = convert.toNativeEvent(event->events) | EPOLLRDHUP | EPOLLIN | EPOLLET;
    epollEvent.data.ptr = event;

    auto result = epoll_ctl(epoll, EPOLL_CTL_MOD, event->fd, &epollEvent);
    return (result == 0 || (result == -1 && errno == EEXIST));
}