#pragma once
#include <sese/concurrent/CASDefine.h>
#include <atomic>

namespace sese::concurrent {

    template<typename T>
    class LinkedQueue {
    public:
        virtual ~LinkedQueue() {
        }

        [[nodiscard]] bool empty() const { return head.load() == nullptr; }

        [[nodiscard]] size_t size() const { return 0; }

        void push(T &&value) {
        }

        void push(const T &value) {
        }

        T pop(T &&value) {
            return value;
        }

        T pop(const T &value) {
            return value;
        }

    private:
        std::atomic<Node<T> *> head;
        std::atomic<Node<T> *> tail;
    };
}// namespace sese::concurrent