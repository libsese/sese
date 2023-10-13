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

/// 非阻塞线程安全栈
/// \tparam T 模板类型
template<class T>
class LinkedStack {
    struct Node {
        T value;
        std::atomic<Node *> next{nullptr};
    };

    std::atomic<Node *> root{nullptr};

public:
    ~LinkedStack() {
        auto pNode = root.load();
        while (pNode) {
            auto pNext = pNode->next.load();
            delete pNode;
            pNode = pNext;
        }
    }

    void push(const T &value) {
        auto newNode = new Node;
        newNode->value = value;

        Node *currentRoot;
        while (true) {
            currentRoot = root.load();
            newNode->next.store(currentRoot);
            if (root.compare_exchange_weak(currentRoot, newNode)) {
                break;
            }
        }
    }

    bool pop(T &value) {
        Node *oldRoot;
        Node *newRoot;
        while (true) {
            oldRoot = root.load();
            if (oldRoot == nullptr) {
                return false;
            }
            newRoot = oldRoot->next.load();
            if (root.compare_exchange_weak(oldRoot, newRoot)) {
                break;
            }
        }
        value = oldRoot->value;
        delete oldRoot;
        return true;
    }

    bool empty() {
        return root.load() == nullptr;
    }
};

} // namespace sese::concurrent