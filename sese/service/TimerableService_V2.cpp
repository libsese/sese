#include <sese/service/TimerableService_V2.h>

using namespace sese::event;
using namespace sese::service;

void TimerableService_V2::dispatch(uint32_t timeout) {
    timeWheel.check();
    EventLoop::dispatch(timeout);
}

void TimerableService_V2::onTimeout(TimeoutEvent_V2 *event) {
}

TimeoutEvent_V2 *TimerableService_V2::setTimeoutEvent(int64_t seconds, void *data) {
    auto event = new TimeoutEvent_V2;
    event->event = timeWheel.delay(
            [this, event]() {
                this->onTimeout(event);
            },
            seconds,
            false
    );
    event->data = data;
    return event;
}

void TimerableService_V2::cancelTimeoutEvent(sese::service::TimeoutEvent_V2 *event) {
    timeWheel.cancel(event->event);
    delete event;
}