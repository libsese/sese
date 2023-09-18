#include "sese/service/TimerableService_V1.h"
#include "sese/event/Event.h"

// #include "sese/record/Marco.h"

#include <algorithm>
#include <chrono>
#include <vector>

sese::service::TimerableService_V1::~TimerableService_V1() {
    if (!timeoutMap.empty()) {
        std::for_each(timeoutMap.begin(), timeoutMap.end(), [](std::pair<const int, TimeoutEvent_V1 *> pair) -> void {
            delete pair.second;
        });
        timeoutMap.clear();
    }
}

void sese::service::TimerableService_V1::onTimeout(sese::service::TimeoutEvent_V1 *timeoutEvent) {
}

sese::service::TimeoutEvent_V1 *sese::service::TimerableService_V1::createTimeoutEvent(int fd, void *data, uint64_t seconds) {
    auto event = new TimeoutEvent_V1;
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

void sese::service::TimerableService_V1::setTimeoutEvent(sese::service::TimeoutEvent_V1 *timeoutEvent, uint64_t seconds) {
    // 原先存在事件，先取消
    {
        auto index = (timeoutEvent->exceptTimestamp - startTimestamp) % 60;
        // SESE_INFO("cancel event at %d", (int) index);
        auto &table = timeoutTable[index];
        table.remove(timeoutEvent);
    }
    // auto iterator = std::find_if(table.begin(), table.end(), [&](TimeoutEvent *event) -> bool {
    //     return timeoutEvent->fd == event->fd;
    // });
    // if (iterator != table.end()) {
    //     table.erase(iterator);
    // }

    // 设置新事件
    {

        timeoutEvent->exceptTimestamp = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()).time_since_epoch().count() + seconds;
        auto index = (timeoutEvent->exceptTimestamp - startTimestamp) % 60;
        auto &table = timeoutTable[index];
        // SESE_INFO("new event at %d", (int) index);
        table.emplace_back(timeoutEvent);
    }
}

sese::service::TimeoutEvent_V1 *sese::service::TimerableService_V1::getTimeoutEventByFd(int fd) {
    auto iterator = timeoutMap.find(fd);
    if (iterator == timeoutMap.end()) {
        return nullptr;
    } else {
        return iterator->second;
    }
}

void sese::service::TimerableService_V1::cancelTimeoutEvent(sese::service::TimeoutEvent_V1 *timeoutEvent) {
    // 原先存在事件，先取消
    auto index = (timeoutEvent->exceptTimestamp - startTimestamp) % 60;
    // SESE_INFO("cancel event at %d", (int) index);
    auto &table = timeoutTable[index];

    table.remove(timeoutEvent);
    // auto iterator = std::find_if(table.begin(), table.end(), [&](TimeoutEvent *event) -> bool {
    //     return timeoutEvent->fd == event->fd;
    // });
    // if (iterator != table.end()) {
    //     table.erase(iterator);
    // }
}

void sese::service::TimerableService_V1::freeTimeoutEvent(sese::service::TimeoutEvent_V1 *timeoutEvent) {
    auto index = (timeoutEvent->exceptTimestamp - startTimestamp) % 60;
    auto &table = timeoutTable[index];
    // SESE_INFO("free %d at %d", timeoutEvent->fd, (int) index);
    table.remove(timeoutEvent);

    auto iterator = timeoutMap.find(timeoutEvent->fd);
    if (iterator != timeoutMap.end()) {
        delete iterator->second;
        timeoutMap.erase(iterator);
    }
}

void sese::service::TimerableService_V1::dispatch(uint32_t timeout) {
    auto now = (uint64_t) std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()).time_since_epoch().count();
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

bool sese::service::TimerableService_V1::init() {
    startTimestamp = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()).time_since_epoch().count();
    return event::EventLoop::init();
}
