#include "Util.h"
#include "thread/Thread.h"

static const char *TYPE_MAIN_THREAD = "Main Thread";
static const char *TYPE_NOT_MAIN_THREAD = "Not Main Thread";

void proc() {
    ROOT_INFO("Thread's name = %s, pid = %d",
              sese::ThreadInfo::getCurrentThreadName().c_str(),
              sese::ThreadInfo::getCurrentThreadId())

    auto i = sese::ThreadInfo::getCurrentThread();
    auto msg = i ? TYPE_NOT_MAIN_THREAD : TYPE_MAIN_THREAD;
    ROOT_INFO("Current thread is %s", msg)
}

int main() {
    sese::Thread thread(proc, "SubThread");
    thread.start();
    thread.join();

    ROOT_INFO("Thread's name = %s, pid = %d",
              sese::ThreadInfo::getCurrentThreadName().c_str(),
              sese::ThreadInfo::getCurrentThreadId())

    auto i = sese::ThreadInfo::getCurrentThread();
    auto msg = i ? TYPE_NOT_MAIN_THREAD : TYPE_MAIN_THREAD;
    ROOT_INFO("Current thread is %s", msg)

    return 0;
}