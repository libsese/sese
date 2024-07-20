#include "sese/native/win/event/WSAEventLoop.h"

#include <winsock2.h>

bool sese::event::WSAEventLoop::init() {
    wsaEvent = WSACreateEvent();
    if (WSA_INVALID_EVENT == wsaEvent) return false;
    if (0 >= listenFd) return true;

    if (WSAEventSelect(listenFd, wsaEvent, FD_ACCEPT)) {
        WSACloseEvent(wsaEvent);
        wsaEvent = nullptr;
        return false;
    }

    this->listenEvent = new WSAEvent;
    this->listenEvent->events = EVENT_ERROR;
    this->listenEvent->fd = listenFd;
    this->listenEvent->wsaEvent = wsaEvent;

    sockets[0] = listenFd;
    wsaEvents[0] = listenEvent->wsaEvent;
    events[0] = listenEvent;
    numbers += 1;

    return true;
}

sese::event::WSAEventLoop::~WSAEventLoop() {
    if (wsaEvent) {
        WSACloseEvent(wsaEvent);
        wsaEvent = nullptr;
    }

    if (listenEvent) {
        delete listenEvent;
        listenEvent = nullptr;
    }
}

void sese::event::WSAEventLoop::dispatch(uint32_t timeout) {
    DWORD n_index = WSAWaitForMultipleEvents(numbers, wsaEvents, FALSE, timeout, FALSE);
    if (n_index == WSA_WAIT_FAILED || n_index == WSA_WAIT_TIMEOUT) return;

    n_index -= WSA_WAIT_EVENT_0;
    for (DWORD i = n_index; i < numbers; ++i) {
        n_index = ::WSAWaitForMultipleEvents(1, &wsaEvents[i], TRUE, timeout, FALSE);
        if (n_index == WSA_WAIT_FAILED || n_index == WSA_WAIT_TIMEOUT) continue;

        WSANETWORKEVENTS enum_event;
        WSAEnumNetworkEvents(sockets[i], wsaEvents[i], &enum_event);
        if (enum_event.lNetworkEvents & FD_ACCEPT) {
            if (enum_event.iErrorCode[FD_ACCEPT_BIT] == 0) {
                SOCKET client = accept(sockets[i], nullptr, nullptr);
                if (-1 != client) {
                    onAccept(static_cast<int>(client));
                }
            } else if (enum_event.iErrorCode[FD_ACCEPT_BIT] != 0 && events[i]->events & EVENT_ERROR) {
                onError(events[i]);
            }
        }
        if ((enum_event.lNetworkEvents & FD_CLOSE) && handleClose) {
            // mutex.lock();
            // WSACloseEvent(wsaEvents[i]);
            // memmove(&sockets[i], &sockets[i], (numbers - i - 1) * sizeof(SOCKET));
            // memmove(&wsaEvents[i], &wsaEvents[i], (numbers - i - 1) * sizeof(HANDLE));
            // memmove(&events[i], &events[i], (numbers - i - 1) * sizeof(WSAEvent *));
            // numbers -= 1;
            // mutex.unlock();
            onClose(events[i]);
        }
        if (enum_event.lNetworkEvents & FD_READ) {
            if (enum_event.iErrorCode[FD_READ_BIT] == 0) {
                char buf;
                if (1 == recv(sockets[i], &buf, 1, MSG_PEEK)) {
                    onRead(events[i]);
                }
            } else if (enum_event.iErrorCode[FD_READ_BIT] != 0 && events[i]->events & EVENT_ERROR) {
                onError(events[i]);
            }
        }
        if (enum_event.lNetworkEvents & FD_WRITE) {
            if (enum_event.iErrorCode[FD_WRITE_BIT] == 0) {
                char buf;
                if (0 == send(sockets[i], &buf, 0, 0)) {
                    onWrite(events[i]);
                }
            } else if (enum_event.iErrorCode[FD_WRITE_BIT] != 0 && events[i]->events & EVENT_ERROR) {
                onError(events[i]);
            }
        }
    }
}

void sese::event::WSAEventLoop::onAccept(int fd) {
}

void sese::event::WSAEventLoop::onRead(sese::event::BaseEvent *event) {
}

void sese::event::WSAEventLoop::onWrite(sese::event::BaseEvent *event) {
}

void sese::event::WSAEventLoop::onError(sese::event::BaseEvent *event) {
}

void sese::event::WSAEventLoop::onClose(sese::event::BaseEvent *event) {
}

sese::event::BaseEvent *sese::event::WSAEventLoop::createEvent(int fd, unsigned int events, void *data) {
    WSAEVENT wsa_event = WSACreateEvent();
    if (WSAEventSelect(fd, wsa_event, convert.toNativeEvent(events) | FD_CLOSE)) {
        WSACloseEvent(wsa_event);
        return nullptr;
    }

    auto event = new WSAEvent;
    event->fd = fd;
    event->events = events;
    event->data = data;
    event->wsaEvent = wsa_event;

    // mutex.lock();
    sockets[numbers] = fd;
    wsaEvents[numbers] = wsa_event;
    this->events[numbers] = event;
    numbers += 1;
    // mutex.unlock();

    return event;
}

void sese::event::WSAEventLoop::freeEvent(sese::event::BaseEvent *event) {
    bool found = false;
    unsigned long i = 0;
    for (; i < numbers; ++i) {
        if (event->fd == sockets[i]) {
            found = true;
            break;
        }
    }

    if (found) {
        WSACloseEvent(wsaEvents[i]);
        memmove(&sockets[i], &sockets[i], (numbers - i - 1) * sizeof(SOCKET));
        memmove(&wsaEvents[i], &wsaEvents[i], (numbers - i - 1) * sizeof(HANDLE));
        memmove(&events[i], &events[i], (numbers - i - 1) * sizeof(WSAEvent *));
        numbers -= 1;
    } else {
        // 这一般不会发生
    }

    delete event;
}

bool sese::event::WSAEventLoop::setEvent(sese::event::BaseEvent *event) {
    auto ev = reinterpret_cast<WSAEvent *>(event);
    auto rt = 0 == WSAEventSelect(ev->fd, ev->wsaEvent, convert.toNativeEvent(ev->events) | FD_CLOSE);
    return rt;
}

void sese::event::WSAEventLoop::setListenFd(int fd) {
    this->listenFd = fd;
}
