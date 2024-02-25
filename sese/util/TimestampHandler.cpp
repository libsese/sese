#include "sese/util/TimestampHandler.h"

sese::TimestampHandler::TimestampHandler(std::chrono::system_clock::time_point point) noexcept {
    latest = std::chrono::duration_cast<std::chrono::milliseconds>(point.time_since_epoch());
}

uint64_t sese::TimestampHandler::getCurrentTimestamp() noexcept {
    latest = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    return latest.count();
}

uint64_t sese::TimestampHandler::tryGetCurrentTimestamp() noexcept {
    auto new_point = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    if (new_point > latest) {
        latest = new_point;
        return latest.count();
    } else {
        if (latest - new_point > std::chrono::seconds(5)) {
            return UINT64_MAX;
        } else {
            return 0;
        }
    }
}