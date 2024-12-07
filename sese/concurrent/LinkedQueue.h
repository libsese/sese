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

/**
 * @file LinkedQueue.h
 * @author Kaoru
 * @date May 30, 2022
 * @brief Non-blocking, thread-safe queue
 * @version 0.2
 * @see https://dl.acm.org/doi/pdf/10.1145/248052.248106
 */

#pragma once

#include <atomic>

namespace sese::concurrent {

// GCOVR_EXCL_START

/// \brief Non-blocking, thread-safe queue
/// \tparam T The template type
template<class T>
class LinkedQueue {
    /// \brief Node
    struct Node {
        std::atomic<T> value{};
        std::atomic<Node *> next{nullptr};
    };

    std::atomic<Node *> head{nullptr};
    std::atomic<Node *> tail{nullptr};

public:
    LinkedQueue() {
        auto node = new Node();
        head.store(node);
        tail.store(node);
    }

    ~LinkedQueue() {
        auto p_node = head.load();
        while (p_node) {
            auto p_next = p_node->next.load();
            if (p_next == p_node) {
                p_next = nullptr;
            }
            delete p_node;
            p_node = p_next;
        }
    }

    void push(const T &value) {
        Node *tail;
        auto node = new Node();
        node->value.store(value);
        node->next.store(nullptr);
        while (true) {
            tail = this->tail.load();
            auto next = tail->next.load();
            if (tail == this->tail) {
                if (next == nullptr) {
                    if (tail->next.compare_exchange_weak(next, node)) {
                        break;
                    }
                } else {
                    this->tail.compare_exchange_weak(tail, next);
                }
            }
        }
        this->tail.compare_exchange_weak(tail, node);
    }

    bool pop(T &value) {
        Node *head;
        while (true) {
            head = this->head.load();
            auto tail = this->tail.load();
            auto next = head->next.load();
            if (head == this->head) {
                if (head == tail) {
                    if (next == nullptr) {
                        return false;
                    }
                    this->tail.compare_exchange_weak(tail, next);
                } else {
                    value = next->value;
                    if (this->head.compare_exchange_weak(head, next)) {
                        break;
                    }
                }
            }
        }
        delete head;
        return true;
    }

    bool empty() {
        return head == tail;
    }
};

// GCOVR_EXCL_STOP

} // namespace sese::concurrent