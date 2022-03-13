#include "DateTime.h"
#ifdef _WIN32

#include <winsock.h>
int gettimeofday(struct timeval *tp, void *tzp) {
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
#else
#include <sys/time.h>
#endif

namespace sese {

    static const int Mon[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    static const int MonLeap[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    DateTime::Ptr DateTime::now(int utc) noexcept {
        timeval tv{};
        gettimeofday(&tv, nullptr);
        DateTime::Ptr dateTime = std::make_shared<DateTime>(tv.tv_sec, utc);

        dateTime->milliseconds = (int) (tv.tv_usec / 1000);
        dateTime->microseconds = (int) (tv.tv_usec - dateTime->milliseconds * 1000);

        return dateTime;
    }

    DateTime::DateTime(long timestamp, int utc) noexcept {
        if (timestamp > 0) {
            timeval tv{timestamp, 0};
            this->timestamp = tv.tv_sec;
            this->utc = utc;
            tv.tv_sec += utc * 60 * 60;
            int days = (int) (tv.tv_sec / (60 * 60 * 24));
            int fourYears = days / (365 * 4 + 1);
            this->years += fourYears * 4 - 1;
            int remain = days % (365 * 4 + 1);
            if (remain < 365 * 1) {
                this->years += 1;
                days = remain;
            } else if (remain < 365 * 2) {
                this->years += 2;
                days = remain - 365 * 1;
            } else if (remain < 365 * 3) {
                this->years += 3;
                days = remain - 365 * 2;
            } else {
                this->years += 4;
                this->isLeap = true;
                days = remain - 365 * 3;
            }

            const int *monList = Mon;
            if (this->isLeap) {
                monList = MonLeap;
            }

            for (int i = 0; i < 12; i++) {
                days -= monList[i];
                if (days < 0) {
                    this->months = i + 1;
                    this->days = days + monList[i];
                    break;
                }
            }
            this->days += 1;

            this->hours = (int) (tv.tv_sec % (60 * 60 * 24)) / 60 / 60;
            this->minutes = (int) (tv.tv_sec % (60 * 60)) / 60;
            this->seconds = (int) (tv.tv_sec % 60);
        }
    }

    TimeSpan DateTime::operator-(const DateTime &dateTime) const noexcept {
        int micro = microseconds - dateTime.microseconds;
        int mill = 0;
        if (micro < 0) {
            micro += 1000;
            mill -= 1;
        }

        long stamp = 0;
        mill += milliseconds - dateTime.milliseconds;
        if (mill < 0) {
            mill += 1000;
            stamp -= 1;
        }

        stamp += this->timestamp - dateTime.timestamp;
        return {stamp, mill, micro};
    }

    DateTime DateTime::operator-(const TimeSpan &timeSpan) const noexcept {
        int micro = microseconds - timeSpan.getMicroseconds();
        int mill = 0;
        if (micro < 0) {
            micro += 1000;
            mill -= 1;
        }

        long stamp = 0;
        mill += milliseconds - timeSpan.getMilliseconds();
        if (mill < 0) {
            mill += 1000;
            stamp -= 1;
        }

        stamp += this->timestamp - timeSpan.getTimestamp();
        return {stamp, mill, micro};
    }

    DateTime DateTime::operator+(const TimeSpan &timeSpan) const noexcept {
        int micro = microseconds + timeSpan.getMicroseconds();
        int mill = 0;
        if (micro >= 1000) {
            micro -= 1000;
            mill += 1;
        }

        long stamp = 0;
        mill += mill + timeSpan.getMilliseconds();
        if (mill >= 1000) {
            mill -= 1000;
            stamp += 1;
        }

        stamp += this->timestamp + timeSpan.getTimestamp();
        return {stamp, mill, micro};
    }

    DateTime::DateTime(long timestamp, int milliseconds, int microseconds, int utc) noexcept : DateTime(timestamp, utc) {
        this->milliseconds = milliseconds;
        this->microseconds = microseconds;
    }

    int DateTime::compareTo(const DateTime &dateTime) const noexcept {
        if (this->timestamp > dateTime.timestamp) {
            return 1;
        } else if (this->timestamp < dateTime.timestamp) {
            return -1;
        } else {
            int i = (this->milliseconds * 1000 + this->microseconds) - (dateTime.milliseconds * 1000 + dateTime.microseconds);
            if (i > 0) i = 1;
            if (i < 0) i = -1;
            return i;
        }
    }

    int DateTime::unclearCompareTo(const DateTime &dateTime) const noexcept {
        if (this->timestamp > dateTime.timestamp) {
            return 1;
        } else if (this->timestamp < dateTime.timestamp) {
            return -1;
        } else {
            return 0;
        }
    }
}// namespace sese