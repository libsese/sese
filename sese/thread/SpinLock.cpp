#include <sese/thread/SpinLock.h>

using namespace sese;

void SpinLock::lock() {
    bool expect = false;
    while (!flag.compare_exchange_weak(expect, true)) {
        expect = false;
    }
}

void SpinLock::unlock() {
    flag.store(false);
}