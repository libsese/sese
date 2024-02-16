#pragma once

#include <queue>
#include <sese/thread/Locker.h>

namespace sese {

/// 缓存队列（多生产者-多消费者）
template<class T>
class BufferedQueue_NN {
public:
    explicit BufferedQueue_NN(size_t writeLimit) : writeLimit(writeLimit) {}

    bool pop(T &t) {
        Locker locker(readMutex);
        if (readQueue.empty()) {
            writeMutex.lock();
            swap();
            writeMutex.unlock();
            if (readQueue.empty()) {
                return false;
            } else {
                t = readQueue.front();
                readQueue.pop();
                return true;
            }
        } else {
            t = readQueue.front();
            readQueue.pop();
            return true;
        }
    }

    bool push(const T &t) {
        Locker locker(writeMutex);
        if (writeQueue.size() >= writeLimit) {
            return false;
        } else {
            writeQueue.push(t);
            return true;
        }
    }

private:
    void swap() {
        std::swap(q1, q2);
    }

private:
    std::queue<T> q1;
    std::queue<T> q2;

    std::queue<T> &writeQueue = q1;
    std::queue<T> &readQueue = q2;

    std::mutex writeMutex;
    std::mutex readMutex;

    size_t writeLimit;
};

/// 缓存队列（多生产者-单消费者）
template<class T>
using BufferedQueue_N1 = BufferedQueue_NN<T>;

/// 缓存队列（单生产者-多消费者）
template<class T>
using BufferedQueue_1N = BufferedQueue_NN<T>;

/// 缓存队列（单生产者-单消费者）
template<class T>
class BufferedQueue_11 {
public:
    explicit BufferedQueue_11(size_t writeLimit) : writeLimit(writeLimit) {}

    bool pop(T &t) {
        Locker locker(mutex);
        if (queue.empty()) {
            return false;
        } else {
            t = queue.front();
            queue.pop();
            return true;
        }
    }

    bool push(const T &t) {
        Locker locker(mutex);
        if (queue.size() >= writeLimit) {
            return false;
        } else {
            queue.push(t);
            return true;
        }
    }

private:
    std::queue<T> queue;

    std::mutex mutex;

    size_t writeLimit;
};

} // namespace sese