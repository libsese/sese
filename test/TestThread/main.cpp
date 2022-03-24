#include "Util.h"
#include "thread/Thread.h"

#define FILTER_TEST_THREAD "fTHREAD"

static const char *TYPE_MAIN_THREAD = "Main Thread";
static const char *TYPE_NOT_MAIN_THREAD = "Not Main Thread";

void proc() {
    ROOT_INFO(FILTER_TEST_THREAD,
              "Thread's name = %s, pid = %" PRId64,
              sese::Thread::getCurrentThreadName().c_str(),
              sese::Thread::getCurrentThreadId())

    auto i = sese::Thread::getCurrentThread();
    auto msg = i ? TYPE_NOT_MAIN_THREAD : TYPE_MAIN_THREAD;
    ROOT_INFO(FILTER_TEST_THREAD,
              "Current thread is %s",
              msg)

    auto arg = (int *) i->getArgument();
    *arg = 1;
}

int main() {
    int num = 0;
    sese::Thread thread(proc, "SubThread");
    thread.setArgument(&num);
    thread.start();
    if (thread.joinable()) {
        thread.join();
        ROOT_INFO(FILTER_TEST_THREAD,
                  "num = %d",
                  num);
    }

    ROOT_INFO(FILTER_TEST_THREAD,
              "Thread's name = %s, pid = %" PRId64,
              sese::Thread::getCurrentThreadName().c_str(),
              sese::Thread::getCurrentThreadId())

    auto i = sese::Thread::getCurrentThread();
    auto msg = i ? TYPE_NOT_MAIN_THREAD : TYPE_MAIN_THREAD;
    ROOT_INFO(FILTER_TEST_THREAD,
              "Current thread is %s",
              msg)

    return 0;
}