#include <sese/service/TimerableService_V2.h>

using namespace sese::event;
using namespace sese::service::v2;

void TimerableService::dispatch(uint32_t timeout) {
    timeWheel.check();
    EventLoop::dispatch(timeout);
}

void TimerableService::onTimeout(v2::TimeoutEvent *event) {
}

TimeoutEvent *TimerableService::setTimeoutEvent(int64_t seconds, void *data) {
    auto event = new v2::TimeoutEvent;
    event->event = timeWheel.delay(
            [this, event]() {
                this->onTimeout(event);
                delete event;
            },
            seconds,
            false
    );
    event->data = data;
    return event;
}

void TimerableService::cancelTimeoutEvent(v2::TimeoutEvent *event) {
    timeWheel.cancel(event->event);
    delete event;
}