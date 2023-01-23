/**
 * @file LinkedStack.h
 * @brief 非阻塞线程安全栈
 * @author kaoru
 * @date 2022年5月31日
 */
#pragma once
#include <sese/concurrent/CASDefine.h>

namespace sese::concurrent {

    /**
     * @brief 非阻塞线程安全栈
     * @tparam T 数据类型
     */
    template<typename T>
    class LinkedStack {
    public:
        using NodeType = Node<T>;

        virtual ~LinkedStack() noexcept {
            auto toDel = (NodeType *)head;
            while (toDel != nullptr) {
                auto next = toDel->next;
                delete toDel;
                toDel = next;
            }
        }

        [[nodiscard]] size_t size() const noexcept {
            size_t size = 0;
            auto p = head;
            while (p != nullptr) {
                size++;
                p = p->next;
            }
            return size;
        }

        [[nodiscard]] bool empty() const noexcept {
            return head == nullptr;
        }

        void push(const T &value) {
            auto newNode = new NodeType;
            newNode->value = value;
            newNode->next = head;
            while (!compareAndSwapPointer((void *volatile *)&head, newNode->next, newNode)) {
                newNode->next = head;
            }
        }

        void push(T &&value) noexcept {
            auto newNode = new NodeType;
            newNode->value = std::move(value);
            newNode->next = head;
            while (!compareAndSwapPointer((void *volatile *)&head, newNode->next, newNode)) {
                newNode->next = head;
            }

        }

        T pop(const T &defaultValue) noexcept {
            auto node = head;
            if (node != nullptr) {
                while (!compareAndSwapPointer((void *volatile *)&head, node->next, node)) {}
                auto value = node->value;
                delete node;
                return value;
            } else {
                return defaultValue;
            }
        }

        T pop(T &&defaultValue) noexcept {
            auto node = head;
            if (node != nullptr) {
                while (!compareAndSwapPointer((void *volatile *)&head, node->next, node)) {}
                auto value = node->value;
                delete node;
                return value;
            } else {
                return defaultValue;
            }
        }

    private:
        NodeType *head = nullptr;
    };

}// namespace sese::concurrent