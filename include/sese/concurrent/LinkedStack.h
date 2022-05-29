#pragma once
#include <sese/concurrent/CASDefine.h>
#include <atomic>

namespace sese::concurrent {

    template<typename T>
    class LinkedStack {
    public:
        virtual ~LinkedStack() noexcept {
            auto toDel = head.load();
            while (toDel != nullptr) {
                auto next = toDel->next;
                delete toDel;
                toDel = next;
            }
        }

        [[nodiscard]] size_t size() const noexcept {
            size_t size = 0;
            auto p = head.load();
            while (p != nullptr) {
                size++;
                p = p->next;
            }
            return size;
        }

        [[nodiscard]] bool empty() const noexcept {
            return head.load() == nullptr;
        }

        void push(const T &value) {
            auto newNode = new Node<int>;
            newNode->value = value;
            newNode->next = head.load();
            while (!head.compare_exchange_weak(newNode->next, newNode)) {}
        }

        void push(T &&value) noexcept {
            auto newNode = new Node<int>;
            newNode->value = std::move(value);
            newNode->next = head.load();
            while (!head.compare_exchange_weak(newNode->next, newNode)) {}
        }

        T pop(const T &defaultValue) noexcept {
            auto node = head.load();
            if (node != nullptr) {
                while (!head.compare_exchange_weak(node, node->next)) {}
                auto value = node->value;
                delete node;
                return value;
            } else {
                return defaultValue;
            }
        }

        T pop(T &&defaultValue) noexcept {
            auto node = head.load();
            if (node != nullptr) {
                while (!head.compare_exchange_weak(node, node->next)) {}
                auto value = node->value;
                delete node;
                return value;
            } else {
                return defaultValue;
            }
        }

    private:
        std::atomic<Node<T> *> head = nullptr;
    };

}// namespace sese::concurrent