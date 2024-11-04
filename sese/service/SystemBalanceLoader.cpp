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

#include "sese/service/SystemBalanceLoader.h"

#include <algorithm>

sese::service::SystemBalanceLoader::~SystemBalanceLoader() noexcept {
    if (_isStart && !_isStop) {
        stop();
    }

    if (!eventLoopVector.empty()) {
        for (decltype(auto) event_loop: eventLoopVector) {
            delete event_loop;
        }
        eventLoopVector.clear();
    }

    if (!socketVector.empty()) {
        for (decltype(auto) sub_socket: socketVector) {
            sese::net::Socket::close(sub_socket);
        }
        socketVector.clear();
    }
}

void sese::service::SystemBalanceLoader::setThreads(size_t th) noexcept {
    if (!_isStart) {
        threads = std::max<size_t>(th, 1);
    }
}

void sese::service::SystemBalanceLoader::start() noexcept {
    if (eventLoopVector.empty()) return;

    for (size_t i = 0; i < threads; ++i) {
        auto thread = std::make_unique<sese::Thread>(
                [this, event_loop = this->eventLoopVector[i]]() {
                    while (!_isStop) {
                        event_loop->dispatch(100);
                    }
                },
                "SBL_" + std::to_string(i)
        );
        thread->start();
        threadVector.emplace_back(std::move(thread));
    }

    _isStart = true;
}

void sese::service::SystemBalanceLoader::stop() noexcept {
    if (_isStart) {
        _isStop = true;

        for (decltype(auto) th: threadVector) {
            th->join();
        }
        threadVector.clear();

        for (decltype(auto) event_loop: eventLoopVector) {
            delete event_loop;
        }
        eventLoopVector.clear();

        for (decltype(auto) sub_socket: socketVector) {
            sese::net::Socket::close(sub_socket);
        }
        socketVector.clear();
    }
}
