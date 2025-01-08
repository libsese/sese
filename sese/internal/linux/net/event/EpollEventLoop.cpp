// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "sese/net/event/BaseEvent.h"
#include "sese/internal/linux/net/event/EpollEventLoop.h"
#include "sese/internal/linux/net/event/EpollEvent.h"

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

    int number_of_fds = epoll_wait(epoll, events, MAX_EVENT_SIZE, (int) timeout);
    if (-1 == number_of_fds) {
        return;
    }

    for (int i = 0; i < number_of_fds; ++i) {
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
            if (events[i].events & EPOLLERR) {
                onError((BaseEvent *) events[i].data.ptr);
            }
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
                if (events[i].events & EPOLLRDHUP) {
                    onClose((BaseEvent *) events[i].data.ptr);
                    continue;
                }
                if (event->events & EVENT_WRITE) {
                    onWrite((BaseEvent *) events[i].data.ptr);
                }
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

    epoll_event epoll_event{};
    if (handleClose) {
        epoll_event.events = convert.toNativeEvent(events) | EPOLLIN | EPOLLET | EPOLLRDHUP;
    } else {
        epoll_event.events = convert.toNativeEvent(events) | EPOLLIN | EPOLLET;
    }
    epoll_event.data.ptr = event;
    if (-1 == epoll_ctl(epoll, EPOLL_CTL_ADD, fd, &epoll_event)) {
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
    epoll_event epoll_event{};
    if (handleClose) {
        epoll_event.events = convert.toNativeEvent(event->events) | EPOLLIN | EPOLLET | EPOLLRDHUP;
    } else {
        epoll_event.events = convert.toNativeEvent(event->events) | EPOLLIN | EPOLLET;
    }
    epoll_event.data.ptr = event;

    auto result = epoll_ctl(epoll, EPOLL_CTL_MOD, event->fd, &epoll_event);
    return (result == 0 || (result == -1 && errno == EEXIST));
}