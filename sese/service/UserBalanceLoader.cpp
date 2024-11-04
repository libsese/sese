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

#include "sese/service/UserBalanceLoader.h"

#include <algorithm>

sese::service::UserBalanceLoader::~UserBalanceLoader() noexcept {
    if (_isStart && !_isStop) {
        stop();
    }

    if (masterSocketQueueArray) {
        delete[] masterSocketQueueArray;
        masterSocketQueueArray = nullptr;
    }

    if (slaveSocketQueueArray) {
        delete[] slaveSocketQueueArray;
        slaveSocketQueueArray = nullptr;
    }

    if (!eventLoopVector.empty()) {
        for (decltype(auto) event_loop: eventLoopVector) {
            delete event_loop;
        }
        eventLoopVector.clear();
    }

    if (masterEventLoop) {
        delete masterEventLoop;
        masterEventLoop = nullptr;
    }

    if (socket) {
        socket->close();
        delete socket;
        socket = nullptr;
    }
}

void sese::service::UserBalanceLoader::setThreads(size_t th) noexcept {
    if (!_isStart) {
        threads = std::max<size_t>(th, 1);
    }
}

void sese::service::UserBalanceLoader::start() noexcept {
    if (eventLoopVector.empty()) return;

    masterThread = std::make_unique<sese::Thread>(std::bind(&UserBalanceLoader::master, this), "UBL_0"); // NOLINT
    masterThread->start();

    for (size_t i = 0; i < threads; ++i) {
        auto thread = std::make_unique<sese::Thread>(
                std::bind( // NOLINT
                        &UserBalanceLoader::slave,
                        this, eventLoopVector[i],
                        &masterSocketQueueArray[i],
                        &slaveSocketQueueArray[i],
                        &mutexArray[i]
                ),
                "UBL_" + std::to_string(i + 1)
        );
        thread->start();
        threadVector.emplace_back(std::move(thread));
    }

    _isStart = true;
}

void sese::service::UserBalanceLoader::stop() noexcept {
    if (_isStart) {
        _isStop = true;

        masterThread->join();
        masterThread = nullptr;

        for (decltype(auto) th: threadVector) {
            th->join();
        }
        threadVector.clear();

        delete[] mutexArray;
        mutexArray = nullptr;

        for (decltype(auto) event_loop: eventLoopVector) {
            delete event_loop;
        }
        eventLoopVector.clear();

        for (int i = 0; i < threads; ++i) {
            decltype(auto) master_queue = masterSocketQueueArray[i];
            decltype(auto) slave_queue = slaveSocketQueueArray[i];
            while (!master_queue.empty()) {
                const auto CLIENT = master_queue.front();
                master_queue.pop();
                sese::net::Socket::close(CLIENT.fd);
            }
            while (!slave_queue.empty()) {
                const auto CLIENT = slave_queue.front();
                slave_queue.pop();
                sese::net::Socket::close(CLIENT.fd);
            }
        }

        delete[] masterSocketQueueArray;
        delete[] slaveSocketQueueArray;
        this->masterSocketQueueArray = nullptr;
        this->slaveSocketQueueArray = nullptr;

        socket->close();
        delete socket;
        socket = nullptr;
    }
}

void sese::service::UserBalanceLoader::dispatchSocket(sese::socket_t sock, void *data) {
    auto index = sock % threads;
    mutexArray[index].lock();
    masterSocketQueueArray[index].push({sock, data});
    mutexArray[index].unlock();
}

void sese::service::UserBalanceLoader::master() noexcept {
    while (!_isStop) {
        masterEventLoop->dispatch(acceptTimeout);

#ifdef WIN32
        socket_t last = 0;
#endif
        while (!masterEventLoop->socketQueue.empty()) {
            auto fd = masterEventLoop->socketQueue.front();
            masterEventLoop->socketQueue.pop();
#ifdef WIN32
            // Windows Socket 值普遍重复利用且是 4 的整数倍
            int factor = 7;
            if (last == fd) {
                factor = 3;
            }
            auto index = (fd / factor) % threads;
            last = fd;
#else
            auto index = fd % threads;
#endif

            // size_t index;
            // for (size_t i = 0; i < threads; ++i) {
            //     index = std::min<size_t>(masterSocketQueueArray[i].size(), index);
            // }

            mutexArray[index].lock();
            masterSocketQueueArray[index].push({fd, nullptr});
            mutexArray[index].unlock();
        }
    }
}

void sese::service::UserBalanceLoader::slave(sese::event::EventLoop *event_loop, std::queue<SocketStatus> *master_queue, std::queue<SocketStatus> *slave_queue, std::mutex *mutex) noexcept {
    while (!_isStop) {
        mutex->lock();
        if (!master_queue->empty()) {
            master_queue->swap(*slave_queue);
            mutex->unlock();
            while (!slave_queue->empty()) {
                auto client = slave_queue->front();
                slave_queue->pop();
                if (client.data == nullptr) {
                    // 此处假装是某一种实现提供的接入客户端
                    event_loop->onAccept(static_cast<int>(client.fd));
                } else if (onDispatchedCallbackFunction) {
                    onDispatchedCallbackFunction(static_cast<int>(client.fd), event_loop, client.data);
                }
            }
        } else {
            mutex->unlock();
        }
        event_loop->dispatch(dispatchTimeout);
    }
}

void sese::service::MasterEventLoop::onAccept(int fd) {
    this->socketQueue.emplace(fd);
}