#include "sese/thread/Locker.h"
#include "sese/thread/Thread.h"
#include "sese/record/LogHelper.h"

sese::LogHelper helper("fLOCKER");

std::mutex mutex;

void IPv4ServerProc() {
    auto thread = sese::Thread::getCurrentThread();
    auto *num = static_cast<uint32_t *>(thread->getArgument());
    sese::Locker locker(mutex);
    //    mutex.lock();
    for (uint32_t idx = 0; idx < 5000; idx++) {
        (*num)++;
    }
    //    mutex.unlock();
    helper.info("num = %d", *num);
}

int main() {
    uint32_t num = 0;
    sese::Thread thread1(IPv4ServerProc, "sub1");
    thread1.setArgument(&num);
    sese::Thread thread2(IPv4ServerProc, "sub2");
    thread2.setArgument(&num);

    thread1.start();
    thread2.start();
    thread1.join();
    thread2.join();

    helper.info("main thread term");
    return 0;
}