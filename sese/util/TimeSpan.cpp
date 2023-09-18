#include "sese/util/TimeSpan.h"

const sese::TimeSpan sese::TimeSpan::Zero = sese::TimeSpan(0, 0, 0, 0, 0, 0);

sese::TimeSpan::TimeSpan(
        int32_t days,
        int32_t hours,
        int32_t minutes,
        int32_t seconds,
        int32_t milliseconds,
        int32_t microseconds
) noexcept
    : days(days),
      hours(hours),
      minutes(minutes),
      seconds(seconds),
      milliseconds(milliseconds),
      microseconds(microseconds) {

    // this->timestamp += days * 24 * 60 * 60;
    // this->timestamp += hours * 60 * 60;
    // this->timestamp += minutes * 60;
    // this->timestamp += seconds;

    timestamp += days * 24 * 60 * 60 * 1000 * 1000;
    timestamp += hours * 60 * 60 * 1000 * 1000;
    timestamp += minutes * 60 * 1000 * 1000;
    timestamp += seconds * 1000 * 1000;
    timestamp += milliseconds * 1000;
    timestamp += microseconds;
}

sese::TimeSpan::TimeSpan(uint64_t timestamp) noexcept {
    // days = (int32_t) (stamp / 60 / 60 / 24);
    // hours = (int32_t) (stamp / 60 / 60 % 24);
    // minutes = (int32_t) (stamp / 60 % 60);
    // seconds = (int32_t) (stamp % 60);
    // this->milliseconds = (int32_t) (u_sec / 1000);
    // this->microseconds = (int32_t) (u_sec - this->milliseconds * 1000);
    this->timestamp = timestamp;

    days = (int32_t) (timestamp / 1000 / 1000 / 60 / 60 / 24);
    hours = (int32_t) (timestamp / 1000 / 1000 / 60 / 60 % 24);
    minutes = (int32_t) (timestamp / 1000 / 1000 / 60 % 60);
    seconds = (int32_t) (timestamp / 1000 / 1000 % 60);

    timestamp %= 1000 * 1000;
    milliseconds = (int32_t) (timestamp / 1000);
    microseconds = (int32_t) (timestamp % 1000);
}
