#include <sese/util/TimeWheel.h>
#include <sese/util/Range.h>

#include <algorithm>
#include <chrono>

using namespace sese;

TimeWheel::TimeWheel() {
    TimeoutEventSlot *preAddress = nullptr;
    Range<int> range(0, 59);
    std::for_each(range.rbegin(), range.rend(), [&](const int &number) {
        this->slots[number].next = preAddress;
        preAddress = &this->slots[number];
    });
    this->slots[59].next = &this->slots[0];
    startTime = lastCheckTime = getTimestamp();
}

TimeWheel::~TimeWheel() {
    for (TimeoutEventSlot &slot : slots) {
        for (TimeoutEvent *event : slot.events) {
            delete event;
        }
        slot.events.clear();
    }
}

int64_t TimeWheel::getTimestamp() {
    auto point = std::chrono::system_clock::now();
    auto timestamp = std::chrono::time_point_cast<std::chrono::seconds>(point);
    return timestamp.time_since_epoch().count();
}

TimeoutEvent *TimeWheel::delay(const TimeoutEvent::Callback &callback, int64_t seconds, bool repeat) {
    auto current = getTimestamp();
    auto event = new TimeoutEvent;
    event->range = seconds;
    event->repeat = repeat;
    event->callback = callback;
    event->target = current + seconds;

    auto index = (event->target - startTime) % 60;
    slots[index].events.emplace_back(event);

    return event;
}

void TimeWheel::cancel(TimeoutEvent *event) {
    auto index = (event->target - startTime) % 60;
    slots[index].events.remove(event);
}

void TimeWheel::check() {
    auto current = getTimestamp();
    auto numberOfSlots = current - lastCheckTime;
    numberOfSlots = std::min<int64_t>(numberOfSlots, 60);

    auto index = (lastCheckTime - startTime) % 60;
    auto pSlot = &slots[index];
    for (int i = 0; i < numberOfSlots; ++i) {
        for (auto iterator = pSlot->events.begin(); iterator != pSlot->events.end(); ) {
#define EVENT (*iterator)
            if (EVENT->target > current) {
                iterator++;
            } else {
                EVENT->callback();
                if (EVENT->repeat) {
                    EVENT->target = getTimestamp() + EVENT->range;
                    auto newIndex = (EVENT->target - startTime) % 60;
                    slots[newIndex].events.emplace_back(EVENT);
                } else {
                    delete EVENT;
                }
                pSlot->events.erase(iterator++);
            }
#undef EVENT
        }
        pSlot = pSlot->next;
    }
    lastCheckTime = current;
}
