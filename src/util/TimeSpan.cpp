#include "TimeSpan.h"

const sese::TimeSpan sese::TimeSpan::Zero = sese::TimeSpan(0, 0, 0, 0, 0, 0);

sese::TimeSpan::TimeSpan(int days, int hours, int minutes, int seconds, int milliseconds, int microseconds) noexcept
    : days(days), hours(hours), minutes(minutes), seconds(seconds), milliseconds(milliseconds), microseconds(microseconds) {

    this->timestamp += days * 24 * 60 * 60;
    this->timestamp += hours * 60 * 60;
    this->timestamp += minutes * 60;
    this->timestamp += seconds;
}

sese::TimeSpan::TimeSpan(long stamp, int milliseconds, int microseconds) noexcept {
    days = (int) (stamp / 60 / 60 / 24);
    hours = (int) (stamp / 60 / 60 % 24);
    minutes = (int) (stamp / 60 % 60);
    seconds = (int) (stamp % 60);
    this->milliseconds = milliseconds;
    this->microseconds = microseconds;
}
