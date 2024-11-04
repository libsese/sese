// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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