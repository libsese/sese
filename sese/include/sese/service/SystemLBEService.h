#pragma once

#include "sese/event/Event.h"
#include "sese/net/ReusableSocket.h"
#include "sese/thread/Thread.h"

namespace sese::service {
    class SystemLBEService;
}

class sese::service::SystemLBEService {
public:
    using CreateEventLoopFunc = event::EventLoop *();

    virtual ~SystemLBEService() noexcept;

    void setThreads(size_t th) noexcept;

    bool isStart() const { return _isStart; }

    template<class SubEvent = event::EventLoop>
    bool init() noexcept;

    void start() noexcept;

    void stop() noexcept;

protected:
    std::atomic_bool _isStart{false};
    std::atomic_bool _isStop{false};

    size_t threads{2};
    std::vector<socket_t> socketVector;
    std::vector<event::EventLoop *> eventLoopVector;
    std::vector<sese::Thread::Ptr> threadVector;
    sese::net::IPAddress::Ptr address = sese::net::IPv4Address::localhost(8080);
};

template<class SubEvent>
bool sese::service::SystemLBEService::init() noexcept {
    if (address == nullptr) return false;

    sese::net::ReusableSocket reusableSocket(address);
    for (size_t i = 0; i < threads; ++i) {
        auto subSocket = reusableSocket.makeRawSocket();
        if (subSocket == -1) {
            goto free_socket;
        }
    }

    for (size_t i = 0; i < threads; ++i) {
        auto event = new SubEvent;
        event->setListenFd((int) socketVector[i]);
        if (!event->init()) {
            delete event;
            goto free_event;
        }
    }

free_event:
    for (decltype(auto) eventLoop: eventLoopVector) {
        delete eventLoop;
    }
    eventLoopVector.clear();

free_socket:
    for (decltype(auto) subSocket: socketVector) {
        sese::net::Socket::close(subSocket);
    }
    socketVector.clear();
    return false;
}
