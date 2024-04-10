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

} // namespace sese::concurrent