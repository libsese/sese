#include "sese/service/TimerableService_V1.h"
#include "sese/event/Event.h"

#include <algorithm>
#include <chrono>

using sese::service::v1::TimerableService;
using sese::service::v1::TimeoutEvent;

TimerableService::~TimerableService() {
    if (!timeoutMap.empty()) {
        std::for_each(timeoutMap.begin(), timeoutMap.end(), [](std::pair<const int, TimeoutEvent *> pair) -> void {
            delete pair.second;
        });
        timeoutMap.clear();
    }
}

void TimerableService::onTimeout(TimeoutEvent *timeout_event) {
}

TimeoutEvent *TimerableService::createTimeoutEvent(int fd, void *data, uint64_t seconds) {
    auto event = new TimeoutEvent;
    timeoutMap[fd] = event;

    event->fd = fd;
    event->data = data;
    event->exceptTimestamp = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()).time_since_epoch().count() + seconds;

    auto index = (event->exceptTimestamp - startTimestamp) % 60;
    // SESE_INFO("create event at %d", (int) index);
    auto &table = timeoutTable[index];
    table.emplace_back(event);

    return event;
}

void TimerableService::setTimeoutEvent(TimeoutEvent *timeout_event, uint64_t seconds) {
    // 原先存在事件，先取消
    {
        auto index = (timeout_event->exceptTimestamp - startTimestamp) % 60;
        // SESE_INFO("cancel event at %d", (int) index);
        auto &table = timeoutTable[index];
        table.remove(timeout_event);
    }
    // auto iterator = std::find_if(table.begin(), table.end(), [&](TimeoutEvent *event) -> bool {
    //     return timeoutEvent->fd == event->fd;
    // });
    // if (iterator != table.end()) {
    //     table.erase(iterator);
    // }

    // 设置新事件
    {

        timeout_event->exceptTimestamp = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()).time_since_epoch().count() + seconds;
        auto index = (timeout_event->exceptTimestamp - startTimestamp) % 60;
        auto &table = timeoutTable[index];
        // SESE_INFO("new event at %d", (int) index);
        table.emplace_back(timeout_event);
    }
}

TimeoutEvent *TimerableService::getTimeoutEventByFd(int fd) {
    auto iterator = timeoutMap.find(fd);
    if (iterator == timeoutMap.end()) {
        return nullptr;
    } else {
        return iterator->second;
    }
}

void TimerableService::cancelTimeoutEvent(TimeoutEvent *timeout_event) {
    // 原先存在事件，先取消
    auto index = (timeout_event->exceptTimestamp - startTimestamp) % 60;
    // SESE_INFO("cancel event at %d", (int) index);
    auto &table = timeoutTable[index];

    table.remove(timeout_event);
    // auto iterator = std::find_if(table.begin(), table.end(), [&](TimeoutEvent *event) -> bool {
    //     return timeoutEvent->fd == event->fd;
    // });
    // if (iterator != table.end()) {
    //     table.erase(iterator);
    // }
}

void TimerableService::freeTimeoutEvent(TimeoutEvent *timeout_event) {
    auto index = (timeout_event->exceptTimestamp - startTimestamp) % 60;
    auto &table = timeoutTable[index];
    // SESE_INFO("free %d at %d", timeoutEvent->fd, (int) index);
    table.remove(timeout_event);

    auto iterator = timeoutMap.find(timeout_event->fd);
    if (iterator != timeoutMap.end()) {
        delete iterator->second;
        timeoutMap.erase(iterator);
    }
}

void TimerableService::dispatch(uint32_t timeout) {
    auto now = static_cast<uint64_t>(std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()).time_since_epoch().count());
    auto index = (now - startTimestamp) % 60;
    auto &table = timeoutTable[index];

    for (auto iterator = table.begin(); iterator != table.end();) {
        if ((*iterator)->exceptTimestamp <= now) {
            onTimeout(*iterator);
            timeoutMap.erase((*iterator)->fd);
            delete (*iterator);
            table.erase(iterator++);
        } else {
            iterator++;
        }
    }

    event::EventLoop::dispatch(timeout);
}

bool TimerableService::init() {
    startTimestamp = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()).time_since_epoch().count();
    return event::EventLoop::init();
}
