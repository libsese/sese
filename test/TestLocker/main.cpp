#include "sese/thread/Locker.h"
#include "sese/thread/Thread.h"
#include "sese/record/LogHelper.h"

sese::record::LogHelper helper("fLOCKER");// NOLINT

std::mutex mutex;

void proc(uint32_t &num) {
    auto thread = sese::Thread::getCurrentThread();
    //    mutex.lock();
    for (uint32_t idx = 0; idx < 5000; idx++) {
        sese::Locker locker(mutex);
        num++;
    }
    //    mutex.unlock();
    helper.info("num = %d", num);
}

int main() {
    uint32_t num = 0;
    sese::Thread thread1([&num] { proc(num); }, "sub1");
    sese::Thread thread2([&num] { proc(num); }, "sub2");

    thread1.start();
    thread2.start();
    thread1.join();
    thread2.join();

    helper.info("main thread term");
    return 0;
}