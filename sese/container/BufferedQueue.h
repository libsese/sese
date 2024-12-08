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

/// \file BufferedQueue.h
/// \brief Buffered queue
/// \author kaoru
/// \date February 16, 2024

#pragma once

#include <queue>
#include <sese/thread/Locker.h>

namespace sese {

/// Cache queue (multi producer-multi consumer)
template<class T>
class BufferedQueueNn {
public:
    explicit BufferedQueueNn(size_t write_limit) : writeLimit(write_limit) {}

    bool pop(T &t) {
        Locker locker(readMutex);
        if (readQueue.empty()) {
            writeMutex.lock();
            swap();
            writeMutex.unlock();
            if (readQueue.empty()) {
                return false;
            } else {
                t = readQueue.front();
                readQueue.pop();
                return true;
            }
        } else {
            t = readQueue.front();
            readQueue.pop();
            return true;
        }
    }

    bool push(const T &t) {
        Locker locker(writeMutex);
        if (writeQueue.size() >= writeLimit) {
            return false;
        } else {
            writeQueue.push(t);
            return true;
        }
    }

private:
    void swap() {
        std::swap(q1, q2);
    }

private:
    std::queue<T> q1;
    std::queue<T> q2;

    std::queue<T> &writeQueue = q1;
    std::queue<T> &readQueue = q2;

    std::mutex writeMutex;
    std::mutex readMutex;

    size_t writeLimit;
};

/// Cache queue (multi producer-single consumer)
template<class T>
using BufferedQueueN1 = BufferedQueueNn<T>;

/// Cache queue (single producer-multi consumer)
template<class T>
using BufferedQueue1N = BufferedQueueNn<T>;

/// Cache queue (single producer-single consumer)
template<class T>
class BufferedQueue11 {
public:
    explicit BufferedQueue11(size_t write_limit) : writeLimit(write_limit) {}

    bool pop(T &t) {
        Locker locker(mutex);
        if (queue.empty()) {
            return false;
        } else {
            t = queue.front();
            queue.pop();
            return true;
        }
    }

    bool push(const T &t) {
        Locker locker(mutex);
        if (queue.size() >= writeLimit) {
            return false;
        } else {
            queue.push(t);
            return true;
        }
    }

private:
    std::queue<T> queue;

    std::mutex mutex;

    size_t writeLimit;
};

} // namespace sese