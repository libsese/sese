#include <gtest/gtest.h>
#include <sese/thread/Thread.h>
#include <sese/container/BufferedQueue.h>
#include <sese/record/Marco.h>
#include <sese/util/StopWatch.h>

TEST(TestBufferedQueue, NN) {
    const auto total = 4000;
    sese::BufferedQueue_NN<int> queue(total);

    std::mutex mutex1;
    auto proc1 = [&] {
        sese::StopWatch watch;
        for (int i = 0; i < total / 2; ++i) {
            mutex1.lock();
            EXPECT_TRUE(queue.push(i));
            mutex1.unlock();
        }
        auto time = watch.stop();
        SESE_INFO("time cost: %zu ms", static_cast<size_t>(time.getTotalMilliseconds()));
    };
    sese::Thread thread1(proc1, "producer1");
    sese::Thread thread2(proc1, "producer2");

    std::mutex mutex2;
    auto times = 0;
    auto proc2 = [&] {
        sese::StopWatch watch;
        int value;
        for (int i = 0; i < total / 2;) {
            if (queue.pop(value)) {
                i += 1;
                mutex2.lock();
                times += 1;
                mutex2.unlock();
            } else {
                continue;
            }
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

TEST(TestBufferedQueue, 11) {
    const auto total = 4000;
    sese::BufferedQueue_NN<int> queue(total);

    std::mutex mutex1;
    auto proc1 = [&] {
        sese::StopWatch watch;
        for (int i = 0; i < total; ++i) {
            mutex1.lock();
            EXPECT_TRUE(queue.push(i));
            mutex1.unlock();
        }
        auto time = watch.stop();
        SESE_INFO("time cost: %zu ms", static_cast<size_t>(time.getTotalMilliseconds()));
    };
    sese::Thread thread1(proc1, "producer1");

    std::mutex mutex2;
    auto times = 0;
    auto proc2 = [&] {
        sese::StopWatch watch;
        int value;
        for (int i = 0; i < total;) {
            if (queue.pop(value)) {
                i += 1;
                mutex2.lock();
                times += 1;
                mutex2.unlock();
            } else {
                continue;
            }
        }
        auto time = watch.stop();
        SESE_INFO("time cost: %zu ms", static_cast<size_t>(time.getTotalMilliseconds()));
    };
    sese::Thread thread2(proc2, "consumer1");

    thread1.start();
    thread2.start();
    thread1.join();
    thread2.join();
    EXPECT_EQ(times, total);
}

TEST(TestBufferedQueue, Limit) {
    const auto limit = 5;
    sese::BufferedQueue_N1<int> queue1(limit);
    sese::BufferedQueue_11<int> queue2(limit);
    for (int i = 0; i < limit; ++i) {
        EXPECT_TRUE(queue1.push(i));
        EXPECT_TRUE(queue2.push(i));
    }
    EXPECT_FALSE(queue1.push(1));
    EXPECT_FALSE(queue2.push(1));
}