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

#include "sese/util/DateTime.h"
#include "sese/util/TimeSpan.h"

#include <chrono>

#ifdef WIN32
#pragma warning(disable : 4996)
#endif

namespace sese {

DateTime DateTime::now(int32_t utc, Policy policy) noexcept {
    auto point = std::chrono::system_clock::now();
    auto timestamp = std::chrono::time_point_cast<std::chrono::microseconds>(point);
    return DateTime(timestamp.time_since_epoch().count(), utc, policy);
}

DateTime::Ptr DateTime::nowPtr(int32_t utc, Policy policy) noexcept {
    auto point = std::chrono::system_clock::now();
    auto timestamp = std::chrono::time_point_cast<std::chrono::microseconds>(point);
    return std::make_unique<DateTime>(timestamp.time_since_epoch().count(), utc, policy);
}

DateTime::DateTime(uint64_t timestamp, int32_t utc, Policy policy) noexcept {
    this->utc = utc;
    this->timestamp = timestamp;

    if (policy == Policy::FORMAT) {
        auto total_seconds = timestamp / 1000 / 1000;
        total_seconds += utc * 60 * 60;

        {
#ifndef SESE_PLATFORM_APPLE
            auto time = static_cast<int64_t>(total_seconds);
#else
            auto time = static_cast<time_t>(total_seconds);
#endif
#ifdef SESE_PLATFORM_WINDOWS
            tm tm{};
            gmtime_s(&tm, &time);
            this->years = tm.tm_year + 1900;
            this->months = tm.tm_mon + 1;
            this->days = tm.tm_mday;
            this->hours = tm.tm_hour;
            this->minutes = tm.tm_min;
            this->seconds = tm.tm_sec;
            this->dayofweek = tm.tm_wday;
            this->dayofyear = tm.tm_yday;
#else
            auto tm = gmtime(reinterpret_cast<const time_t *>(&time));
            this->years = tm->tm_year + 1900;
            this->months = tm->tm_mon + 1;
            this->days = tm->tm_mday;
            this->hours = tm->tm_hour;
            this->minutes = tm->tm_min;
            this->seconds = tm->tm_sec;
            this->dayofweek = tm->tm_wday;
            this->dayofyear = tm->tm_yday;
#endif
        }

        {
            auto y = this->years;
            auto u = timestamp % (1000 * 1000);
            this->isLeap = ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0);
            this->milliseconds = static_cast<int32_t>(static_cast<int64_t>(u) / 1000);
            this->microseconds = static_cast<int32_t>(static_cast<int64_t>(u) % 1000);
        }
    }
}

int32_t DateTime::compareTo(const DateTime &date_time) const noexcept {
    auto rt = static_cast<int64_t>(this->timestamp) - static_cast<int64_t>(date_time.timestamp);
    if (rt < 0) {
        return -1;
    } else if (rt > 0) {
        return 1;
    } else {
        return 0;
    }
}

int32_t DateTime::unclearCompareTo(const DateTime &date_time) const noexcept {
    auto rt = static_cast<int64_t>(this->timestamp) - static_cast<int64_t>(date_time.timestamp);
    rt /= 1000 * 1000;
    if (rt < 0) {
        return -1;
    }
    if (rt > 0) {
        return 1;
    }
    return 0;
}

TimeSpan DateTime::operator-(const DateTime &date_time) const noexcept {
    auto time = this->timestamp - date_time.timestamp;
    return TimeSpan{time};
}

DateTime DateTime::operator-(const TimeSpan &time_span) const noexcept {
    return DateTime{this->timestamp - time_span.getTimestamp(), this->utc};
}

DateTime DateTime::operator+(const TimeSpan &time_span) const noexcept {
    return DateTime{this->timestamp + time_span.getTimestamp(), this->utc};
}
} // namespace sese