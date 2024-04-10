/// \file RingQueue.h
/// \brief 环形队列
/// \author kaoru
/// \date 2024/02/17
/// \version 1.0

#pragma once

#include <stdexcept>

namespace sese {

/**
 * @brief 环形队列
 * @tparam T 元素类型
 * @tparam N 队列大小
 */
template<typename T, int N>
class RingQueue {
public:
    // 获取队头元素，元素不足则抛出异常
    T &front() {
        if (empty()) {
            throw std::out_of_range("queue is empty");
        }
        return _data[_head];
    }

    // 出队，元素不足则抛出异常
    void pop() {
        if (empty()) {
            throw std::out_of_range("queue is empty");
        }
        _head = (_head + 1) % N;
        _size -= 1;
    }

    // 入队，队列已满则抛出异常
    void push(const T &data) {
        if (full()) {
            throw std::out_of_range("queue is full");
        }
        _data[_tail] = data;
        _tail = (_tail + 1) % N;
        _size += 1;
    }

    // 判断队列是否为空
    [[nodiscard]] bool empty() const {
        return _size == 0;
    }

    // 判断队列是否已满
    [[nodiscard]] bool full() const {
        return _size == N;
    }

    // 获取队列大小
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