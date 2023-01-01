#include "DateTime.h"

#ifdef _WIN32
#include <ctime>
int32_t getTimeOfDate(struct timeval *tp, struct timezone *tzp) {
    time_t clock;
    struct tm tm {};
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    tm.tm_year = wtm.wYear - 1900;
    tm.tm_mon = wtm.wMonth - 1;
    tm.tm_mday = wtm.wDay;
    tm.tm_hour = wtm.wHour;
    tm.tm_min = wtm.wMinute;
    tm.tm_sec = wtm.wSecond;
    tm.tm_isdst = -1;
    clock = mktime(&tm);
    tp->tv_sec = (long) clock;
    tp->tv_usec = wtm.wMilliseconds * 1000;
    return 0;
}
#endif
#ifdef __linux__
#include <sys/time.h>
int32_t getTimeOfDate(struct timeval *tp, struct timezone *tzp) {
    return gettimeofday(tp,tzp);
}
#endif
#ifdef __APPLE__
#include <sys/time.h>
int32_t getTimeOfDate(struct timeval *tp, struct timezone *tzp) {
    return gettimeofday(tp, tzp);
}
#endif

static const int MonDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static const int MonLeapDays[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

namespace sese {

    DateTime::Ptr DateTime::now(int32_t utc) noexcept {
        timeval tv{};
        getTimeOfDate(&tv);
        DateTime::Ptr dateTime = std::make_unique<DateTime>(tv.tv_sec, utc);

        dateTime->milliseconds = (int32_t) (tv.tv_usec / 1000);
        dateTime->microseconds = (int32_t) (tv.tv_usec - dateTime->milliseconds * 1000);

        return dateTime;
    }

    DateTime::DateTime(int64_t timestamp, int32_t utc) noexcept {
        if (timestamp > 0) {
            timeval tv{static_cast<long>(timestamp), 0};
            this->timestamp = tv.tv_sec;
            this->utc = utc;
            tv.tv_sec += utc * 60 * 60;
            int tempDays = (int) (tv.tv_sec / (60 * 60 * 24));
            this->dayofweek = (tempDays + 4) % 7;
            int fourYears = tempDays / (365 * 4 + 1);
            this->years += fourYears * 4 - 1;
            int remain = tempDays % (365 * 4 + 1);
            if (remain < 365 * 1) {
                this->years += 1;
                tempDays = remain;
            } else if (remain < 365 * 2) {
                this->years += 2;
                tempDays = remain - 365 * 1;
            } else if (remain < 365 * 3) {
                this->years += 3;
                tempDays = remain - 365 * 2;
            } else {
                this->years += 4;
                this->isLeap = true;
                tempDays = remain - 365 * 3;
            }

            const int *monList = MonDays;
            if (this->isLeap) {
                monList = MonLeapDays;
            }

            for (int i = 0; i < 12; i++) {
                tempDays -= monList[i];
                if (tempDays < 0) {
                    this->months = i + 1;
                    this->days = tempDays + monList[i];
                    break;
                }
            }
            this->days += 1;

            this->hours = (int) (tv.tv_sec % (60 * 60 * 24)) / 60 / 60;
            this->minutes = (int) (tv.tv_sec % (60 * 60)) / 60;
            this->seconds = (int) (tv.tv_sec % 60);
        }
    }

    DateTime::DateTime(int64_t timestamp, int64_t u_sec, int utc) noexcept : DateTime(timestamp, utc) {
        this->milliseconds = (int32_t) (u_sec / 1000);
        this->microseconds = (int32_t) (u_sec - this->milliseconds * 1000);
    }

    int32_t DateTime::compareTo(const DateTime &dateTime) const noexcept {
        if (this->timestamp > dateTime.timestamp) {
            return 1;
        } else if (this->timestamp < dateTime.timestamp) {
            return -1;
        } else {
            auto i = (int32_t) ((this->u_sec) - (dateTime.u_sec));
            if (i > 0) i = 1;
            if (i < 0) i = -1;
            return i;
        }
    }

    int32_t DateTime::unclearCompareTo(const DateTime &dateTime) const noexcept {
        if (this->timestamp > dateTime.timestamp) {
            return 1;
        } else if (this->timestamp < dateTime.timestamp) {
            return -1;
        } else {
            return 0;
        }
    }

    TimeSpan DateTime::operator-(const DateTime &dateTime) const noexcept {
        int64_t stamp = 0;
        int64_t uSec = this->u_sec - dateTime.u_sec;
        if (uSec < 0) {
            uSec += 86400000000;
            stamp -= 1;
        }

        stamp += this->timestamp - dateTime.timestamp;
        return {stamp, uSec};
    }

    DateTime DateTime::operator-(const TimeSpan &timeSpan) const noexcept {
        int64_t stamp = 0;
        int64_t uSec = this->u_sec - timeSpan.getUSecond();
        if (uSec < 0) {
            uSec += 86400000000;
            stamp -= 1;
        }

        stamp += this->timestamp - timeSpan.getTimestamp();
        return {stamp, uSec};
    }

    DateTime DateTime::operator+(const TimeSpan &timeSpan) const noexcept {
        int64_t stamp = 0;
        int64_t uSec = this->u_sec + timeSpan.getUSecond();
        if (uSec > 86400000000) {
            uSec -= 86400000000;
            stamp += 1;
        }

        stamp += this->timestamp + timeSpan.getTimestamp();
        return {stamp, uSec};
    }
}// namespace sese