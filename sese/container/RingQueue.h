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

/// \file RingQueue.h
/// \brief Ring queue
/// \author kaoru
/// \date 2024/02/17
/// \version 1.0

#pragma once

#include <stdexcept>

namespace sese {

/**
 * @brief Ring queue
 * @tparam T Element type
 * @tparam N Queue size
 */
template<typename T, int N>
class RingQueue {
public:
    /// @exception std::out_of_range queue is empty
    T &front() {
        if (empty()) {
            throw std::out_of_range("queue is empty");
        }
        return _data[_head];
    }

    /// @exception std::out_of_range queue is empty
    void pop() {
        if (empty()) {
            throw std::out_of_range("queue is empty");
        }
        _head = (_head + 1) % N;
        _size -= 1;
    }

    /// @exception std::out_of_range queue is full
    void push(const T &data) {
        if (full()) {
            throw std::out_of_range("queue is full");
        }
        _data[_tail] = data;
        _tail = (_tail + 1) % N;
        _size += 1;
    }

    /// Is the queue empty
    [[nodiscard]] bool empty() const {
        return _size == 0;
    }

    /// Is the queue full
    [[nodiscard]] bool full() const {
        return _size == N;
    }

    /// Get queue size
    [[nodiscard]] size_t size() const {
        return _size;
    }

private:
    T _data[N];
    size_t _head{};
    size_t _tail{};
    size_t _size{};
};
} // namespace sese