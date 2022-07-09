#include <sese/concurrent/LinkedStack.h>
#include <sese/concurrent/LinkedQueue.h>
#include <sese/thread/ThreadPool.h>
#include <sese/record/LogHelper.h>
#include <sese/Test.h>
#include <sese/Util.h>

using sese::LogHelper;
using sese::Test;
using sese::Thread;
using sese::ThreadPool;
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
    ThreadPool pool("PUSH", 8);
    for(int i = 0; i < 200; i++) {
        pool.postTask([&queue](){
            queue.push(0);
        });
    }
    for(int i = 0; i < 210; i++) {
        pool.postTask([&queue](){
            queue.pop(0);
        });
    }
    sese::sleep(5);
    pool.shutdown();
    auto i = queue.size();
    assert(helper, i == 0, 0)
}

int main() {
    //    testCompareAndSwap();
    //    testLinkedStack();
    testLinkedQueue();

    return 0;
}