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

#include <sese/native/darwin/event/KqueueEventLoop.h>

#include <unistd.h>
#include <sys/event.h>
#include <sys/socket.h>

#include <cmath>

#define MAX_EVENT_SIZE 64

bool sese::event::KqueueEventLoop::init() {
    kqueue = ::kqueue();
    if (-1 == kqueue) return false;
    if (0 >= listenFd) return true;

    this->listenEvent = new KqueueEvent;
    this->listenEvent->fd = listenFd;
    this->listenEvent->events = EVENT_NULL;
    this->listenEvent->data = nullptr;

    struct kevent kevent {};
    EV_SET(&kevent, listenFd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, this->listenEvent);
    if (-1 == ::kevent(kqueue, &kevent, 1, nullptr, 0, nullptr)) {
        delete this->listenEvent;
        this->listenEvent = nullptr;

        close(this->kqueue);
        this->kqueue = -1;

        return false;
    }
    return true;
}

sese::event::KqueueEventLoop::~KqueueEventLoop() {
    if (-1 != kqueue) {
        close(kqueue);
        kqueue = -1;
    }

    if (listenEvent) {
        delete listenEvent;
        listenEvent = nullptr;
    }
}

void sese::event::KqueueEventLoop::dispatch(uint32_t time) {
    struct kevent events[MAX_EVENT_SIZE]{};
    auto div = ldiv(time, 1000);
    struct timespec timeout {
        div.quot, div.rem * 1000000
    };

    auto number_of_fds = ::kevent(kqueue, nullptr, 0, events, MAX_EVENT_SIZE, &timeout);
    if (-1 == number_of_fds) return;
    for (int i = 0; i < number_of_fds; ++i) {
        auto event = reinterpret_cast<BaseEvent *>(events[i].udata);
        if (events[i].ident == listenFd) {
            auto client = accept(listenFd, nullptr, nullptr);
            if (client == -1) continue;
            onAccept(client);
            continue;
        } else {
            if (events[i].flags & EV_ERROR && event->events & EVENT_ERROR) {
                onError(event);
                continue;
            }
            if (events[i].filter == EVFILT_READ) {
                if (event->events & EVENT_READ) {
                    onRead(event);
                }

                if ((events[i].flags & EV_EOF) && handleClose) {
                    onClose(event);
                    continue;
                }
            } else if (events[i].filter == EVFILT_WRITE) {
                if ((events[i].flags & EV_EOF) && handleClose) {
                    onClose(event);
                    continue;
                }

                if (event->events & EVENT_WRITE) {
                    onWrite(reinterpret_cast<BaseEvent *>(events[i].udata));
                }
            }
        }
    }
}

void sese::event::KqueueEventLoop::onAccept(int fd) {
}

void sese::event::KqueueEventLoop::onRead(sese::event::BaseEvent *event) {
}

void sese::event::KqueueEventLoop::onWrite(sese::event::BaseEvent *event) {
}

void sese::event::KqueueEventLoop::onError(sese::event::BaseEvent *event) {
}

void sese::event::KqueueEventLoop::onClose(sese::event::BaseEvent *event) {
}

// bool sese::event::KqueueEventLoop::addNativeEvent(int fd, uint32_t ev, void *data) const {
//     struct kevent kevent {};
//     EV_SET(&kevent, fd, ev, EV_ADD | EV_ENABLE | EV_ONESHOT, 0, 0, data);
//     return 0 == ::kevent(kqueue, &kevent, 1, nullptr, 0, nullptr);
// }
//
// bool sese::event::KqueueEventLoop::delNativeEvent(int fd, uint32_t ev, void *data) const {
//     struct kevent kevent {};
//     EV_SET(&kevent, fd, ev, EV_DELETE, 0, 0, data);
//     return 0 == ::kevent(kqueue, &kevent, 1, nullptr, 0, nullptr);
// }

sese::event::BaseEvent *sese::event::KqueueEventLoop::createEvent(int fd, unsigned int events, void *data) {
    auto event = new KqueueEvent;
    event->fd = fd;
    event->events = events;
    // event->oldEvents = events;
    event->data = data;

    struct kevent kevent {};
    if (events & EVENT_READ) {
        EV_SET(&kevent, event->fd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_ONESHOT, 0, 0, event);
        ::kevent(kqueue, &kevent, 1, nullptr, 0, nullptr);
    }
    if (events & EVENT_WRITE) {
        EV_SET(&kevent, event->fd, EVFILT_WRITE, EV_ADD | EV_ENABLE | EV_ONESHOT, 0, 0, event);
        ::kevent(kqueue, &kevent, 1, nullptr, 0, nullptr);
    }

    return event;
}

void sese::event::KqueueEventLoop::freeEvent(sese::event::BaseEvent *event) {
    // auto fd = event->fd;
    // auto events = event->events;
    // if (events & EVENT_READ) {
    //    delNativeEvent(fd, EVFILT_READ, event);
    // }
    // delNativeEvent(fd, EVFILT_READ, event);
    // if (events & EVENT_WRITE) {
    //     delNativeEvent(fd, EVFILT_WRITE, event);
    // }

    delete event;
}

bool sese::event::KqueueEventLoop::setEvent(sese::event::BaseEvent *event) {
    auto e = reinterpret_cast<KqueueEvent *>(event);
    auto events = e->events;

    struct kevent kevent {};
    if (events & EVENT_READ) {
        EV_SET(&kevent, event->fd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_ONESHOT, 0, 0, event);
        ::kevent(kqueue, &kevent, 1, nullptr, 0, nullptr);
    }
    if (events & EVENT_WRITE) {
        EV_SET(&kevent, event->fd, EVFILT_WRITE, EV_ADD | EV_ENABLE | EV_ONESHOT, 0, 0, event);
        ::kevent(kqueue, &kevent, 1, nullptr, 0, nullptr);
    }

    return event;
}