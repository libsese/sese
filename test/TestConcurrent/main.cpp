#include <sese/concurrent/LinkedStack.h>
#include <sese/concurrent/LinkedQueue.h>
#include <sese/thread/Thread.h>
#include <sese/record/LogHelper.h>
#include <sese/Test.h>

using sese::LogHelper;
using sese::Test;
using sese::Thread;
using sese::concurrent::LinkedQueue;
using sese::concurrent::LinkedStack;

LogHelper helper("fCONCURRENT");// NOLINT

void testCompareAndSwap() {
    int *object = nullptr;
    int *newValue = new int(200);
    auto rt = compareAndSwapPointer((void *volatile *) &object, nullptr, newValue);
    Test::assert(helper, rt);
    Test::assert(helper, *object == *newValue);
    delete newValue;
}

void testLinkedStack() {
    LinkedStack<int> list;
    Thread th1([&list]() {
        for (int i = 0; i < 100; i += 2) {
            list.push(i);
        }
    },
               "thread1");
    Thread th2([&list]() {
        for (int i = 1; i < 100; i += 2) {
            list.push(i);
        }
    },
               "thread2");

    th1.start();
    th2.start();

    th1.join();
    th2.join();

    Test::assert(helper, list.size() == 100);
}

int main() {
    testCompareAndSwap();
    testLinkedStack();
    // testLinkedQueue();

    return 0;
}