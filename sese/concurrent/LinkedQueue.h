/**
 * @file LinkedQueue.h
 * @author kaoru
 * @date 2022年5月30日
 * @brief 非阻塞线程安全队列
 * @bug 队列长度超过10^3时可能产生意想不到的错误，暂时原因不明
 * @ref https://github.com/openjdk/jdk/blob/master/src/java.base/share/classes/java/util/concurrent/ConcurrentLinkedQueue.java
 */
#pragma once
#include <sese/concurrent/CASDefine.h>

namespace sese::concurrent {

    /**
     * @brief 非阻塞线程安全队列
     * @tparam T 数据类型
     */
    template<typename T>
    class LinkedQueue {
    public:
        using NodeType = Node<T>;

        virtual ~LinkedQueue() {
            auto toDel = (NodeType *)head;
            while (toDel != nullptr) {
                auto next = toDel->next;
                delete toDel;
                toDel = next;
            }
        }

        [[nodiscard]] bool empty() const { return head == nullptr; }

        [[nodiscard]] size_t size() const {
            size_t size = 0;
            auto p = (NodeType *) head;
            while (p != nullptr) {
                size++;
                p = p->next;
            }
            return size;
        }

        void push(T &&value) {
            auto newNode = new NodeType;
            newNode->value = value;

            if (head == nullptr) {
                // 队列空
                // 优先更新 head 节点
                while (!compareAndSwapPointer((void *volatile *) &head, nullptr, newNode)) {}
                compareAndSwapPointer((void *volatile *) &tail, nullptr, newNode);
            } else if (tail == nullptr) {
                // 可能 tail 没能更新成功导致仍为 nullptr
                // 故从 head 开始查找尾部位置
                // assert(head != nullptr)
                auto realTail = findTail((NodeType *) head);
                while (!compareAndSwapPointer((void *volatile *) &realTail, nullptr, newNode)) {}
                compareAndSwapPointer((void *volatile *) &tail, nullptr, newNode);
            } else {
                // 正常插入，tail 不一定是尾部节点，但一定是最靠近尾部的
                // 故从 tail 开始查找尾部位置
                // assert(tail != nullptr)
                auto realTail = findTail((NodeType *) tail);
                while (!compareAndSwapPointer((void *volatile *) &realTail->next, nullptr, newNode)) {}
                compareAndSwapPointer((void *volatile *) &tail, realTail, newNode);
            }
        }

        void push(const T &value) {
            auto newNode = new NodeType;
            newNode->value = value;

            if (head == nullptr) {
                // 队列空
                // 优先更新 head 节点
                while (!compareAndSwapPointer((void *volatile *) &head, nullptr, newNode)) {}
                compareAndSwapPointer((void *volatile *) &tail, nullptr, newNode);
            } else if (tail == nullptr) {
                // 可能 tail 没能更新成功导致仍为 nullptr
                // 故从 head 开始查找尾部位置
                // assert(head != nullptr)
                auto realTail = findTail((NodeType *) head);
                while (!compareAndSwapPointer((void *volatile *) &realTail, nullptr, newNode)) {}
                compareAndSwapPointer((void *volatile *) &tail, nullptr, newNode);
            } else {
                // 正常插入，tail 不一定是尾部节点，但一定是最靠近尾部的
                // 故从 tail 开始查找尾部位置
                // assert(tail != nullptr)
                auto realTail = findTail((NodeType *) tail);
                while (!compareAndSwapPointer((void *volatile *) &realTail->next, nullptr, newNode)) {}
                compareAndSwapPointer((void *volatile *) &tail, realTail, newNode);
            }
        }

        T pop(T &&defaultValue) {
            if (head == nullptr) {
                // 队列空，直接返回缺省值
                return defaultValue;
            } else {
                NodeType *node = (NodeType *) head;
                //bug: may be head was nullptr
                while (!compareAndSwapPointer((void *volatile *) &head, node, node->next)) {
                    node = (NodeType *) head;
                }
                return node->value;
            }
        }

        T pop(const T &defaultValue) {
            if (head == nullptr) {
                // 队列空，直接返回缺省值
                return defaultValue;
            } else {
                NodeType *node = (NodeType *) head;
                //bug: may be head was nullptr
                while (!compareAndSwapPointer((void *volatile *) &head, node, node->next)) {
                    node = (NodeType *) head;
                }
                return node->value;
            }
        }

    private:
        NodeType *findTail(NodeType *from) {
            if (from == nullptr) {
                return (NodeType *) head;
            }
            while (from->next != nullptr) {
                from = from->next;
            }
            return from;
        }

        volatile NodeType *head = nullptr;
        volatile NodeType *tail = nullptr;
    };
}// namespace sese::concurrent