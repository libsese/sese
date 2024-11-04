// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#define SESE_C_LIKE_FORMAT

#include <sese/concurrent/LinkedStack.h>
#include <sese/concurrent/LinkedQueue.h>
#include <sese/concurrent/ObjectPool.h>
#include <sese/thread/Async.h>
#include <sese/record/Marco.h>

#include <gtest/gtest.h>

TEST(TestConcurrent, DISABLED_LinkedStack_Push) {
    using sese::concurrent::LinkedStack;

    LinkedStack<int> stack;
    auto proc = [&stack] {
        for (int i = 0; i < 10000; ++i) {
            stack.push(1);
        }
    };

    auto f1 = sese::async<void>(proc);
    auto f2 = sese::async<void>(proc);
    auto f3 = sese::async<void>(proc);
    auto f4 = sese::async<void>(proc);
    f1.get();
    f2.get();
    f3.get();
    f4.get();

    int count = 0;
    int value;
    while (stack.pop(value)) {
        count += 1;
    }
    EXPECT_EQ(count, 40000);
}

TEST(TestConcurrent, DISABLED_LinkedStack_PushAndPop) {
    using sese::concurrent::LinkedStack;

    std::atomic<int> produce = 0;
    std::atomic<int> consume = 0;
    const int EXPECT = 20000;
    LinkedStack<int> stack;
    auto produce_proc = [&stack, &produce] {
        for (int i = 0; i < 10000; ++i) {
            produce += 1;
            stack.push(1);
            // SESE_DEBUG("PUSH");
        }
    };
    auto consume_proc = [&stack, &consume] {
        int value;
        while (true) {
            if (stack.pop(value)) {
                consume += 1;
                // SESE_DEBUG("OK");
            } else {
                SESE_WARN("FAILED");
            }
            if (consume == EXPECT) {
                break;
            }
        }
    };

    auto p1 = sese::async<void>(produce_proc);
    auto p2 = sese::async<void>(produce_proc);

    auto c1 = sese::async<void>(consume_proc);
    auto c2 = sese::async<void>(consume_proc);

    p1.get();
    c1.get();
    p2.get();
    c2.get();

    EXPECT_EQ(produce, consume);
}

TEST(TestConcurrent, DISABLED_LinkedQueue_Push) {
    using sese::concurrent::LinkedQueue;

    LinkedQueue<int> queue;
    auto proc = [&queue] {
        for (int i = 0; i < 10000; ++i) {
            queue.push(1);
        }
    };

    auto f1 = sese::async<void>(proc);
    auto f2 = sese::async<void>(proc);
    auto f3 = sese::async<void>(proc);
    auto f4 = sese::async<void>(proc);
    f1.get();
    f2.get();
    f3.get();
    f4.get();

    int count = 0;
    int value;
    while (queue.pop(value)) {
        count += 1;
    }
    EXPECT_EQ(count, 40000);
}

TEST(TestConcurrent, DISABLED_LinkedQueue_PushAndPop) {
    using sese::concurrent::LinkedQueue;

    std::atomic<int> produce = 0;
    std::atomic<int> consume = 0;
    const int EXPECT = 20000;
    LinkedQueue<int> queue;
    auto produce_proc = [&queue, &produce] {
        for (int i = 0; i < 10000; ++i) {
            produce += 1;
            queue.push(1);
            // SESE_DEBUG("PUSH");
        }
    };
    auto consume_proc = [&queue, &consume] {
        int value;
        while (true) {
            if (queue.pop(value)) {
                consume += 1;
                // SESE_DEBUG("OK");
            } else {
                SESE_WARN("FAILED");
            }
            if (consume == EXPECT) {
                break;
            }
        }
    };

    auto p1 = sese::async<void>(produce_proc);
    auto p2 = sese::async<void>(produce_proc);

    auto c1 = sese::async<void>(consume_proc);
    auto c2 = sese::async<void>(consume_proc);

    p1.get();
    c1.get();
    p2.get();
    c2.get();

    EXPECT_EQ(produce, consume);
}

TEST(TestConcurrent, DISABLED_ObjectPool) {
    using sese::concurrent::ObjectPool;

    ObjectPool<int32_t>::ObjectPtr j{};
    {
        auto pool = ObjectPool<int32_t>::create();
        auto i = pool->borrow();
        *i = 10;
        j = pool->borrow();
        *j = 10;
    }
}