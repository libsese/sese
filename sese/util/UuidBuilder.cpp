#include "UuidBuilder.h"

#include <thread>

sese::UuidBuilder::UuidBuilder() noexcept
    : timestampHandler(std::chrono::system_clock::now()) {
}

bool sese::UuidBuilder::generate(uuid::Uuid &dest) noexcept {
    auto timestamp = timestampHandler.tryGetCurrentTimestamp();
    if (timestamp == 0) {
        for (int i = 0; i < 5; i++) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            timestamp = timestampHandler.tryGetCurrentTimestamp();
            if (timestamp == 0) {
                continue;
            } else if (timestamp == UINT64_MAX) {
                return false;
            } else {
                dest.setTimestamp(timestamp);
                return true;
            }
        }
        return false;
    } else if (timestamp == UINT64_MAX) {
        return false;
    } else {
        dest.setTimestamp(timestamp);
        return true;
    }
}