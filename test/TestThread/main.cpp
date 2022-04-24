#include "sese/record/LogHelper.h"
#include "sese/thread/Thread.h"
#include "sese/Test.h"

sese::LogHelper helper("fTHREAD"); // NOLINT

static const char *TYPE_MAIN_THREAD = "Main Thread";
static const char *TYPE_NOT_MAIN_THREAD = "Not Main Thread";

void IPv4ServerProc() {
    helper.info("Thread's name = %s, tid = %" PRIdTid,
                sese::Thread::getCurrentThreadName(),
                sese::Thread::getCurrentThreadId());

    auto i = sese::Thread::getCurrentThread();
    auto msg = i ? TYPE_NOT_MAIN_THREAD : TYPE_MAIN_THREAD;
    helper.info("Current thread is %s", msg);

    sese::Test::assert(helper, i != nullptr, -1);
    auto arg = reinterpret_cast<int *>(i->getArgument());
    *arg = 1;
}

int main() {
    int num = 0;
    sese::Thread thread(IPv4ServerProc, "SubThread");
    thread.setArgument(&num);
    thread.start();
    if (thread.joinable()) {
        thread.join();
        helper.info("num = %d", num);
    }

    helper.info("Thread's name = %s, tid = %" PRIdTid,
                sese::Thread::getCurrentThreadName(),
                sese::Thread::getCurrentThreadId());

    auto i = sese::Thread::getCurrentThread();
    auto msg = i ? TYPE_NOT_MAIN_THREAD : TYPE_MAIN_THREAD;
    helper.info("Current thread is %s", msg);

    return 0;
}