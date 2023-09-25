#include "sese/util/TimestampHandler.h"

sese::TimestampHandler::TimestampHandler(std::chrono::system_clock::time_point point) noexcept {
    latest = std::chrono::duration_cast<std::chrono::milliseconds>(point.time_since_epoch());
}

uint64_t sese::TimestampHandler::getCurrentTimestamp() noexcept {
    latest = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    return latest.count();
}

uint64_t sese::TimestampHandler::tryGetCurrentTimestamp() noexcept {
    auto newPoint = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    if (newPoint > latest) {
        latest = newPoint;
        return latest.count();
    } else {
        if (latest - newPoint > std::chrono::seconds(5)) {
            return UINT64_MAX;
        } else {
            return 0;
        }
    }
}