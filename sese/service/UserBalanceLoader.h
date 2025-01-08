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

/// \file UserBalanceLoader.h
/// \brief User Load Balancer
/// \author kaoru
/// \version 0.1
/// \date June 5, 2023

#pragma once

#include "sese/net/event/Event.h"
#include "sese/net/Socket.h"
#include "sese/thread/Thread.h"

#include <atomic>
#include <queue>
#include <mutex>

namespace sese::service {

class MasterEventLoop;

/// User load balancer, applicable to all platforms
class UserBalanceLoader {
public:
    ~UserBalanceLoader() noexcept;

    /// Set the number of threads used by the loader
    /// \param th Number of threads
    void setThreads(size_t th) noexcept;

    /// Set the service startup address
    /// \param addr IP Address
    void setAddress(const net::IPAddress::Ptr &addr) noexcept { UserBalanceLoader::address = addr; } // GCOVR_EXCL_LINE

    /// Set the timeout for the main listening thread
    /// \param to Timeout in milliseconds
    void setAcceptTimeout(uint32_t to) noexcept { UserBalanceLoader::acceptTimeout = to; }

    /// Set the dispatch timeout for worker threads
    /// \param to Timeout in milliseconds
    void setDispatchTimeout(uint32_t to) noexcept { UserBalanceLoader::dispatchTimeout = to; }

    /// Get the current loader status
    /// \return Loader status
    [[nodiscard]] bool isStarted() const { return _isStart; } // GCOVR_EXCL_LINE

    /// Initialize loader resources
    /// \tparam SERVICE Service to be started
    /// \return Whether initialization is successful
    template<class SERVICE>
    bool init() noexcept;

    /// Initialize balancer resources
    /// \tparam SERVICE Service to be started
    /// \param creator Service creation function, returns an instance pointer on success, otherwise should return nullptr
    /// \return Whether initialization is successful
    template<class SERVICE>
    bool init(std::function<SERVICE *()> creator) noexcept;

    /// Start the current loader and service
    void start() noexcept;

    /// Stop the current loader and unload services
    void stop() noexcept;

    void dispatchSocket(socket_t sock, void *data);

    void setOnDispatchedCallbackFunction(const std::function<void(int, sese::event::EventLoop *, void *)> &cb) { UserBalanceLoader::onDispatchedCallbackFunction = cb; }

protected:
    /// Socket status
    struct SocketStatus {
        socket_t fd{};
        void *data{};
    };

    void master() noexcept;

    void slave(
            sese::event::EventLoop *event_loop,
            std::queue<SocketStatus> *master_queue,
            std::queue<SocketStatus> *slave_queue,
            std::mutex *mutex
    ) noexcept;

protected:
    std::atomic_bool _isStart{false};
    std::atomic_bool _isStop{false};

    uint32_t acceptTimeout = 100;
    uint32_t dispatchTimeout = 100;
    size_t threads{2};
    std::vector<sese::event::EventLoop *> eventLoopVector;
    std::vector<sese::Thread::Ptr> threadVector;
    sese::net::IPAddress::Ptr address = sese::net::IPv4Address::localhost(8080);

    sese::net::Socket *socket{nullptr};
    sese::service::MasterEventLoop *masterEventLoop{nullptr};
    sese::Thread::Ptr masterThread{nullptr};
    /// socket_t exchange queues
    std::queue<SocketStatus> *masterSocketQueueArray{nullptr};
    std::queue<SocketStatus> *slaveSocketQueueArray{nullptr};
    std::mutex *mutexArray{nullptr};

    std::function<void(int, sese::event::EventLoop *event_loop, void *)> onDispatchedCallbackFunction;
};

/// User load balancer main thread
class MasterEventLoop final : public sese::event::EventLoop {
public:
    void onAccept(int fd) override;

    std::queue<socket_t> socketQueue;
};
} // namespace sese::service

// The test code here is not convenient to simulate
// GCOVR_EXCL_START

template<class SERVICE>
bool sese::service::UserBalanceLoader::init() noexcept {
    return sese::service::UserBalanceLoader::init<SERVICE>([]() -> SERVICE * { return new SERVICE; });
}

template<class SERVICE>
bool sese::service::UserBalanceLoader::init(std::function<SERVICE *()> creator) noexcept {
    if (address == nullptr) return false;

    socket = new net::Socket(
            address->getRawAddress()->sa_family == AF_INET ? net::Socket::Family::IPv4 : net::Socket::Family::IPv6,
            net::Socket::Type::TCP
    );
    if (-1 == socket->getRawSocket()) {
        return false;
    }

    if (!socket->setNonblocking()) {
        goto freeSocket;
    }

    if (0 != socket->bind(address)) {
        goto freeSocket;
    }

    if (0 != socket->listen(32)) {
        goto freeSocket;
    }

    masterEventLoop = new MasterEventLoop;
    masterEventLoop->setListenFd(static_cast<int>(socket->getRawSocket()));
    if (!masterEventLoop->init()) {
        goto freeMaster;
    }

    for (size_t i = 0; i < threads; ++i) {
        auto event = creator();
        if (event == nullptr) {
            goto freeEvent;
        }
        if (!event->init()) {
            delete event;
            goto freeEvent;
        } else {
            eventLoopVector.emplace_back(event);
        }
    }

    // Initialize exchange queues
    masterSocketQueueArray = new std::queue<SocketStatus>[threads];
    slaveSocketQueueArray = new std::queue<SocketStatus>[threads];
    mutexArray = new std::mutex[threads];

    return true;

freeEvent:
    for (decltype(auto) event_loop: eventLoopVector) {
        delete event_loop;
    }
    eventLoopVector.clear();

freeMaster:
    delete masterEventLoop;
    masterEventLoop = nullptr;

freeSocket:
    socket->close();
    delete socket;
    socket = nullptr;
    return false;
}


// GCOVR_EXCL_STOP