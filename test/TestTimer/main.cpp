#include <sese/Timer.h>
#include <sese/record/LogHelper.h>
#include <sese/Util.h>

using sese::LogHelper;
using sese::Timer;

LogHelper helper("fTIMER");// NOLINT

void foo() {
    helper.debug("foo time up");
}

void bar() {
    helper.debug("bar time up");
}

int main() {
    auto timer = Timer::create(5);
    helper.debug("timer start");
    timer->delay(foo, 2);
    auto task = timer->delay(bar, 1, true);
    sese::sleep(4);
//    task->cancel();
    sese::sleep(3);
    helper.debug("timer thread joining");
    timer->shutdown();
    return 0;
}