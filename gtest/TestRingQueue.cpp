#include <gtest/gtest.h>
#include <sese/thread/Thread.h>
#include <sese/container/BufferedQueue.h>
#include <sese/record/Marco.h>
#include <sese/util/StopWatch.h>
#include <sese/container/RingQueue.h>

using sese::RingQueue;

TEST(TestRingQueue, PushAndPop) {
    RingQueue<int, 3> queue;
    queue.push(1);
    queue.push(2);
    queue.push(3);
    ASSERT_TRUE(queue.full());
    ASSERT_EQ(queue.size(), 3);
    ASSERT_EQ(queue.front(), 1);
    queue.pop();
    ASSERT_EQ(queue.front(), 2);
    ASSERT_FALSE(queue.empty());
    queue.push(4);
}

TEST(TestRingQueue, Empty) {
    RingQueue<int, 3> queue;
    ASSERT_TRUE(queue.empty());
    queue.push(1);
    ASSERT_FALSE(queue.empty());
}

TEST(TestRingQueue, Full) {
    RingQueue<int, 3> queue;
    ASSERT_FALSE(queue.full());
    queue.push(1);
    queue.push(2);
    queue.push(3);
    ASSERT_TRUE(queue.full());
}

TEST(TestRingQueue, Pop) {
    RingQueue<int, 3> queue;
    queue.push(1);
    queue.push(2);
    queue.push(3);
    ASSERT_EQ(queue.front(), 1);
    queue.pop();
    ASSERT_EQ(queue.front(), 2);
    queue.pop();
    ASSERT_EQ(queue.front(), 3);
    queue.pop();
    ASSERT_TRUE(queue.empty());
    ASSERT_THROW(queue.front(), std::out_of_range);
    ASSERT_THROW(queue.pop(), std::out_of_range);
}

TEST(TestRingQueue, Size) {
    RingQueue<int, 3> queue;
    ASSERT_EQ(queue.size(), 0);
    queue.push(1);
    queue.push(2);
    ASSERT_EQ(queue.size(), 2);
}

TEST(TestRingQueue, Push) {
    RingQueue<int, 3> queue;
    queue.push(1);
    queue.push(2);
    queue.push(3);
    ASSERT_THROW(queue.push(1), std::out_of_range);
}

TEST(TestRingQueue, MutilThread) {
    const auto total = 4000;
    RingQueue<int, 500> queue;

    std::mutex mutex;
    auto proc1 = [&] {
        sese::StopWatch watch;
        for (int i = 0; i < total / 2;) {
            mutex.lock();
            if (!queue.full()) {
                queue.push(i);
                i += 1;
            } else  {
                SESE_DEBUG("full hit");
            }
            mutex.unlock();
        }
        auto time = watch.stop();
        SESE_INFO("time cost: %zu ms", static_cast<size_t>(time.getTotalMilliseconds()));
    };
    sese::Thread thread1(proc1, "producer1");
    sese::Thread thread2(proc1, "producer2");

    auto times = 0;
    auto proc2 = [&] {
        sese::StopWatch watch;
        for (int i = 0; i < total / 2;) {
            mutex.lock();
            if (!queue.empty()) {
                queue.pop();
                i += 1;
                times += 1;
            } else {
                SESE_DEBUG("empty hit");
            }
            mutex.unlock();
        }
        auto time = watch.stop();
        SESE_INFO("time cost: %zu ms", static_cast<size_t>(time.getTotalMilliseconds()));
    };
    sese::Thread thread3(proc2, "consumer1");
    sese::Thread thread4(proc2, "consumer2");

    thread1.start();
    thread2.start();
    thread3.start();
    thread4.start();
    thread1.join();
    thread2.join();
    thread3.join();
    thread4.join();
    ASSERT_EQ(times, total);
}