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
 * @file LinkedStack.h
 * @brief 非阻塞线程安全栈
 * @author kaoru
 * @date 2022年5月31日
 * @version 0.2
 */

#pragma once

#include <atomic>

namespace sese::concurrent {

// GCOVR_EXCL_START

/// \brief 非阻塞线程安全栈
/// \tparam T 模板类型
template<class T>
class LinkedStack {
    /// \brief 节点
    struct Node {
        T value;
        std::atomic<Node *> next{nullptr};
    };

    std::atomic<Node *> root{nullptr};

public:
    ~LinkedStack() {
        auto p_node = root.load();
        while (p_node) {
            auto p_next = p_node->next.load();
            delete p_node;
            p_node = p_next;
        }
    }

    void push(const T &value) {
        auto new_node = new Node;
        new_node->value = value;

        Node *current_root;
        while (true) {
            current_root = root.load();
            new_node->next.store(current_root);
            if (root.compare_exchange_weak(current_root, new_node)) {
                break;
            }
        }
    }

    bool pop(T &value) {
        Node *old_root;
        Node *new_root;
        while (true) {
            old_root = root.load();
            if (old_root == nullptr) {
                return false;
            }
            new_root = old_root->next.load();
            if (root.compare_exchange_weak(old_root, new_root)) {
                break;
            }
        }
        value = old_root->value;
        delete old_root;
        return true;
    }

    bool empty() {
        return root.load() == nullptr;
    }
};

// GCOVR_EXCL_STOP

} // namespace sese::concurrent