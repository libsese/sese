#include "sese/service/SystemLBEService.h"

#include <algorithm>

sese::service::SystemLBEService::~SystemLBEService() noexcept {
    if (_isStart && !_isStop) {
        stop();
    }
}

void sese::service::SystemLBEService::setThreads(size_t th) noexcept {
    threads = std::max<size_t>(th, 2);
}

void sese::service::SystemLBEService::start() noexcept {
    if (eventLoopVector.empty()) return;

    for (size_t i = 0; i < threads; ++i) {
        auto thread = std::make_unique<sese::Thread>([this, eventLoop = this->eventLoopVector[i]]() {
            while (!_isStop) {
                eventLoop->dispatch(100);
            }
        },
                                                     "SystemLBThread_" + std::to_string(i));
        thread->start();
        threadVector.emplace_back(std::move(thread));
    }

    _isStart = true;
}

void sese::service::SystemLBEService::stop() noexcept {
    if (_isStart) {
        _isStop = true;

        for (decltype(auto) th: threadVector) {
            th->join();
        }
        threadVector.clear();

        for (decltype(auto) eventLoop: eventLoopVector) {
            delete eventLoop;
        }
        eventLoopVector.clear();

        for (decltype(auto) subSocket: socketVector) {
            sese::net::Socket::close(subSocket);
        }
        socketVector.clear();
    }
}
