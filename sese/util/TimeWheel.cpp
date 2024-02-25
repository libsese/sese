#include <sese/util/TimeWheel.h>
#include <sese/util/Range.h>

#include <algorithm>
#include <chrono>

using namespace sese;

TimeWheel::TimeWheel() {
    TimeoutEventSlot *pre_address = nullptr;
    Range<int> range(0, 59);
    std::for_each(range.rbegin(), range.rend(), [&](const int &number) {
        this->slots[number].next = pre_address;
        pre_address = &this->slots[number];
    });
    this->slots[59].next = &this->slots[0];
    startTime = lastCheckTime = getTimestamp();
}

TimeWheel::~TimeWheel() {
    for (TimeoutEventSlot &slot: slots) {
        for (TimeoutEvent *event: slot.events) {
            delete event; // GCOVR_EXCL_LINE
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
    delete event; // GCOVR_EXCL_LINE
}

void TimeWheel::check() {
    auto current = getTimestamp();
    auto number_of_slots = current - lastCheckTime;
    number_of_slots = std::min<int64_t>(number_of_slots, 60);

    auto index = (lastCheckTime - startTime) % 60;
    auto p_slot = &slots[index];
    for (int i = 0; i < number_of_slots; ++i) {
        for (auto iterator = p_slot->events.begin(); iterator != p_slot->events.end();) {
#define EVENT (*iterator)
            if (EVENT->target > current) {
                iterator++;
            } else {
                EVENT->callback();
                if (EVENT->repeat) {
                    EVENT->target = getTimestamp() + EVENT->range;
                    auto new_index = (EVENT->target - startTime) % 60;
                    slots[new_index].events.emplace_back(EVENT);
                } else {
                    delete EVENT; // GCOVR_EXCL_LINE
                }
                p_slot->events.erase(iterator++);
            }
#undef EVENT
        }
        p_slot = p_slot->next;
    }
    lastCheckTime = current;
}
