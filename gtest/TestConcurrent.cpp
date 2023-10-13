#include <sese/concurrent/LinkedStack.h>
#include <sese/thread/Async.h>
#include <sese/record/Marco.h>

#include <gtest/gtest.h>

TEST(TestConcurrent, LinkedStack_Push) {
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

TEST(TestConcurrent, LinkedStack_PushAndPop) {
    using sese::concurrent::LinkedStack;

    std::atomic<int> produce = 0;
    std::atomic<int> consume = 0;
    const int expect = 20000;
    LinkedStack<int> stack;
    auto produceProc = [&stack, &produce] {
        for (int i = 0; i < 10000; ++i) {
            produce += 1;
            stack.push(1);
            // SESE_DEBUG("PUSH");
        }
    };
    auto consumeProc = [&stack, &consume] {
        int value;
        while (true) {
            if (stack.pop(value)) {
                consume += 1;
                // SESE_DEBUG("OK");
            } else {
                SESE_WARN("FAILED");
            }
            if (consume == expect) {
                break;
            }
        }
    };

    auto p1 = sese::async<void>(produceProc);
    auto p2 = sese::async<void>(produceProc);

    auto c1 = sese::async<void>(consumeProc);
    auto c2 = sese::async<void>(consumeProc);

    p1.get();
    c1.get();
    p2.get();
    c2.get();

    EXPECT_EQ(produce, consume);
}