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

/// \file SystemBalanceLoader.h
/// \brief System Balance Loader
/// \author kaoru
/// \version 0.1
/// \date June 3, 2023

#pragma once

#include "sese/event/Event.h"
#include "sese/net/ReusableSocket.h"
#include "sese/thread/Thread.h"

#include <atomic>
#include <functional>
#include <mutex>

namespace sese::service {

/// \brief System Balance Loader (non-user scheduled loader)
/// \warning This loader is effective only on Linux
/// \see sese::service::BalanceLoader
class SystemBalanceLoader final {
public:
    ~SystemBalanceLoader() noexcept;

    /// Set the number of threads used by the loader
    /// \param th Number of threads
    void setThreads(size_t th) noexcept;

    /// Set the service startup address
    /// \param addr IP Address
    void setAddress(const net::IPAddress::Ptr &addr) noexcept { SystemBalanceLoader::address = addr; }

    /// Set the timeout for dispatching from threads
    /// \param to Timeout in milliseconds
    void setAcceptTimeout(uint32_t to) noexcept { SystemBalanceLoader::timeout = to; }

    /// Set the timeout for dispatching from threads
    /// \param to Timeout in milliseconds
    void setDispatchTimeout(uint32_t to) noexcept { SystemBalanceLoader::timeout = to; }

    /// Get the current loader status
    /// \return The status of the loader
    [[nodiscard]] bool isStarted() const { return _isStart; }

    /// Initialize loader resources
    /// \tparam SERVICE The service to be started
    /// \return Whether the initialization was successful
    template<class SERVICE>
    bool init() noexcept;

    /// Initialize balancer resources
    /// \tparam SERVICE The service to be started
    /// \param creator Service creation function, which returns an instance pointer if creation is successful, otherwise should return nullptr indicating failure
    /// \return Whether the initialization was successful
    template<class SERVICE>
    bool init(std::function<SERVICE *()> creator) noexcept;

    /// Start the current loader and service
    void start() noexcept;

    /// Shut down the current loader and unload the service
    void stop() noexcept;

protected:
    std::atomic_bool _isStart{false};
    std::atomic_bool _isStop{false};

    uint32_t timeout = 100;
    size_t threads{2};
    std::vector<socket_t> socketVector;
    std::vector<event::EventLoop *> eventLoopVector;
    std::vector<sese::Thread::Ptr> threadVector;
    sese::net::IPAddress::Ptr address = sese::net::IPv4Address::localhost(8080);
};
} // namespace sese::service

template<class SERVICE>
bool sese::service::SystemBalanceLoader::init() noexcept {
    return sese::service::SystemBalanceLoader::init<SERVICE>([]() -> SERVICE * { return new SERVICE; });
}

// GCOVR_EXCL_START

template<class SERVICE>
bool sese::service::SystemBalanceLoader::init(std::function<SERVICE *()> creator) noexcept {
    if (address == nullptr) return false;

    sese::net::ReusableSocket reusable_socket(address);
    for (size_t i = 0; i < threads; ++i) {
        auto sub_socket = reusable_socket.makeRawSocket();
        if (sub_socket == -1) {
            goto freeSocket;
        }
        if (0 != sese::net::Socket::setNonblocking(sub_socket)) {
            goto freeSocket;
        }
        if (0 != sese::net::Socket::listen(sub_socket, 32)) {
            goto freeSocket;
        }
        socketVector.emplace_back(sub_socket);
    }

    for (size_t i = 0; i < threads; ++i) {
        auto event = creator();
        if (event == nullptr) {
            goto freeEvent;
        }
        event->setListenFd(static_cast<int>(socketVector[i]));
        if (!event->init()) {
            delete event;
            goto freeEvent;
        } else {
            eventLoopVector.emplace_back(event);
        }
    }

    return true;

freeEvent:
    for (decltype(auto) event_loop: eventLoopVector) {
        delete event_loop;
    }
    eventLoopVector.clear();

freeSocket:
    for (decltype(auto) sub_socket: socketVector) {
        sese::net::Socket::close(sub_socket);
    }
    socketVector.clear();
    return false;
}

// GCOVR_EXCL_STOP