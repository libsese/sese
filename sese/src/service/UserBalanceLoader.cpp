#include "sese/service/UserBalanceLoader.h"

#include <algorithm>

sese::service::UserBalanceLoader::~UserBalanceLoader() noexcept {
    if (_isStart && !_isStop) {
        stop();
    }
}

void sese::service::UserBalanceLoader::setThreads(size_t th) noexcept {
    if (!_isStart) {
        threads = std::max<size_t>(th, 1);
    }
}

void sese::service::UserBalanceLoader::start() noexcept {
    if (eventLoopVector.empty()) return;

    masterThread = std::make_unique<sese::Thread>(std::bind(&UserBalanceLoader::master, this), "UserBalanceLoader_0");// NOLINT
    masterThread->start();

    for (size_t i = 0; i < threads; ++i) {
        auto thread = std::make_unique<sese::Thread>(
                std::bind(// NOLINT
                        &UserBalanceLoader::slave,
                        this, eventLoopVector[i],
                        &masterSocketQueueArray[i],
                        &slaveSocketQueueArray[i],
                        &mutexArray[i]
                ),
                "UserBalanceLoader_" + std::to_string(i + 1)
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

        for (decltype(auto) th: threadVector) {
            th->join();
        }
        threadVector.clear();

        for (decltype(auto) eventLoop: eventLoopVector) {
            delete eventLoop;
        }
        eventLoopVector.clear();

        for (int i = 0; i < threads; ++i) {
            decltype(auto) masterQueue = masterSocketQueueArray[i];
            decltype(auto) slaveQueue = slaveSocketQueueArray[i];
            while (!masterQueue.empty()) {
                auto client = masterQueue.front();
                masterQueue.pop();
                sese::net::Socket::close(client);
            }
            while (!slaveQueue.empty()) {
                auto client = slaveQueue.front();
                slaveQueue.pop();
                sese::net::Socket::close(client);
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

void sese::service::UserBalanceLoader::master() noexcept {
    while (!_isStop) {
        masterEventLoop->dispatch(100);

#ifdef WIN32
        socket_t last = 0;
#endif
        while (!masterEventLoop->socketQueue.empty()) {
            auto client = masterEventLoop->socketQueue.front();
            masterEventLoop->socketQueue.pop();
#ifdef WIN32
            // Windows Socket 值普遍重复利用且是 4 的整数倍
            int factor = 7;
            if (last == client) {
                factor = 3;
            }
            auto index = (client / factor) % threads;
            last = client;
#else
            auto index = client % threads;
#endif

            // size_t index;
            // for (size_t i = 0; i < threads; ++i) {
            //     index = std::min<size_t>(masterSocketQueueArray[i].size(), index);
            // }

            mutexArray[index].lock();
            masterSocketQueueArray[index].push(client);
            mutexArray[index].unlock();
        }
    }
}

void sese::service::UserBalanceLoader::slave(sese::event::EventLoop *eventLoop, std::queue<socket_t> *masterQueue, std::queue<socket_t> *slaveQueue, std::mutex *mutex) noexcept {
    while (!_isStop) {
        mutex->lock();
        if (!masterQueue->empty()) {
            masterQueue->swap(*slaveQueue);
            mutex->unlock();
            while (!slaveQueue->empty()) {
                auto client = slaveQueue->front();
                slaveQueue->pop();
                // 此处假装是某一种实现提供的接入客户端
                eventLoop->onAccept((int) client);
            }
        } else {
            mutex->unlock();
        }
        eventLoop->dispatch(100);
    }
}

void sese::service::MasterEventLoop::onAccept(int fd) {
    this->socketQueue.emplace(fd);
}