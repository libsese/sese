#include "sese/service/TimerableService.h"
#include "sese/event/Event.h"

#include <algorithm>
#include <chrono>
#include <vector>

sese::service::TimerableService::~TimerableService() {
    if (!timeoutMap.empty()) {
        std::for_each(timeoutMap.begin(), timeoutMap.end(), [](std::pair<const int, TimeoutEvent *> pair) -> void {
            delete pair.second;
        });
        timeoutMap.clear();
    }
}

void sese::service::TimerableService::onTimeout(sese::service::TimeoutEvent *timeoutEvent) {
}

sese::service::TimeoutEvent *sese::service::TimerableService::createTimeoutEvent(int fd, void *data, uint64_t seconds) {
    auto event = new TimeoutEvent;
    timeoutMap[fd] = event;

    event->fd = fd;
    event->data = data;
    event->exceptTimestamp = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()).time_since_epoch().count() + seconds;

    auto index = (event->exceptTimestamp - startTimestamp) % 60;
    auto &table = timeoutTable[index];
    table.emplace_back(event);

    return event;
}

void sese::service::TimerableService::setTimeoutEvent(sese::service::TimeoutEvent *timeoutEvent, uint64_t seconds) {
    // 原先存在事件，先取消
    auto index = (timeoutEvent->exceptTimestamp - startTimestamp) % 60;
    auto &table = timeoutTable[index];
    table.remove(timeoutEvent);
    // auto iterator = std::find_if(table.begin(), table.end(), [&](TimeoutEvent *event) -> bool {
    //     return timeoutEvent->fd == event->fd;
    // });
    // if (iterator != table.end()) {
    //     table.erase(iterator);
    // }

    // 设置新事件
    timeoutEvent->exceptTimestamp = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()).time_since_epoch().count() + seconds;
    index = (timeoutEvent->exceptTimestamp - startTimestamp) % 60;
    table = timeoutTable[index];
    table.emplace_back(timeoutEvent);
}

sese::service::TimeoutEvent *sese::service::TimerableService::getTimeoutEventByFd(int fd) {
    auto iterator = timeoutMap.find(fd);
    if (iterator == timeoutMap.end()) {
        return nullptr;
    } else {
        return iterator->second;
    }
}

void sese::service::TimerableService::cancelTimeoutEvent(sese::service::TimeoutEvent *timeoutEvent) {
    // 原先存在事件，先取消
    auto index = (timeoutEvent->exceptTimestamp - startTimestamp) % 60;
    auto &table = timeoutTable[index];

    table.remove(timeoutEvent);
    // auto iterator = std::find_if(table.begin(), table.end(), [&](TimeoutEvent *event) -> bool {
    //     return timeoutEvent->fd == event->fd;
    // });
    // if (iterator != table.end()) {
    //     table.erase(iterator);
    // }
}

void sese::service::TimerableService::freeTimeoutEvent(sese::service::TimeoutEvent *timeoutEvent) {
    auto index = (timeoutEvent->exceptTimestamp - startTimestamp) % 60;
    auto &table = timeoutTable[index];
    table.remove(timeoutEvent);

    auto iterator = timeoutMap.find(timeoutEvent->fd);
    if (iterator != timeoutMap.end()) {
        delete iterator->second;
        timeoutMap.erase(iterator);
    }
}

void sese::service::TimerableService::dispatch(uint32_t timeout) {
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

bool sese::service::TimerableService::init() {
    startTimestamp = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()).time_since_epoch().count();
    return event::EventLoop::init();
}
