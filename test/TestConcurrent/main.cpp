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
    assert(helper, rt, -1);
    assert(helper, *object == *newValue, -2);
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

    assert(helper, list.size() == 100, -3);
}

void testLinkedQueue() {
    LinkedQueue<int> queue;
    Thread th3([&queue]() {
        for (int i = 0; i < 1000; i++) {
            queue.push(i);
        }
    },
               "thread3");
    Thread th4([&queue]() {
        for (int i = 0; i < 1000;) {
            if(queue.pop(-1) != -1) {
                i++;
                helper.info("pop succ");
            } else {
                helper.info("pop fail");
            }
        }
    },
               "thread4");

    th3.start();
    th4.start();

    th3.join();
    th4.join();
}

int main() {
//    testCompareAndSwap();
//    testLinkedStack();
    testLinkedQueue();

    return 0;
}