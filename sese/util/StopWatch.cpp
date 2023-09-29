#include <sese/util/StopWatch.h>

sese::StopWatch::StopWatch()
    : lastStopTime(sese::DateTime::now(0, DateTime::Policy::ONLY_CREATE)) {
}

sese::TimeSpan sese::StopWatch::stop() {
    auto now = sese::DateTime::now(0, DateTime::Policy::ONLY_CREATE);
    auto result = now - lastStopTime;
    lastStopTime = now;
    return result;
}
