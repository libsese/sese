#include "sese/text/DateTimeFormatter.h"
#include "sese/text/StringBuilder.h"
#include "sese/io/InputBufferWrapper.h"

#include <cstring>
#include <ctime>

//#ifdef _WIN32
//#define itoa _itoa_s
//#else
//static char *itoa(int num, char *str, int radix = 10) {
//    char index[] = "0123456789ABCDEF";
//    unsigned unum;
//    int i = 0, j, k;
//    if (radix == 10 && num < 0) {
//        unum = (unsigned) -num;
//        str[i++] = '-';
//    } else
//        unum = (unsigned) num;
//    do {
//        str[i++] = index[unum % (unsigned) radix];
//        unum /= radix;
//    } while (unum);
//    str[i] = '\0';
//    if (str[0] == '-')
//        k = 1;
//    else
//        k = 0;
//
//    for (j = k; j <= (i - 1) / 2; j++) {
//        char temp;
//        temp = str[j];
//        str[j] = str[i - 1 + k - j];
//        str[i - 1 + k - j] = temp;
//    }
//    return str;
//}
//#endif

const char *sese::text::DateTimeFormatter::Month[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

const char *sese::text::DateTimeFormatter::Mon[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

const char *sese::text::DateTimeFormatter::WeekDay[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

const char *sese::text::DateTimeFormatter::WkDay[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};


static int count(const char &ch, const char *str) {
    for (int i = 0; i < 4; i++) {
        if (*(str + i) != ch) {
            return i;
        }
    }
    return 4;
}

std::string sese::text::DateTimeFormatter::format(const sese::DateTime &dateTime, const std::string &pattern) {
    auto builder = StringBuilder(128);
    auto input = sese::io::InputBufferWrapper(pattern.c_str(), pattern.length());// GCOVR_EXCL_LINE

    while (true) {
        char buffer[5]{};
        auto len = input.peek(buffer, 4);// GCOVR_EXCL_LINE
        if (len < 1) break;

        if (buffer[0] == 'd') {
            auto count = ::count('d', buffer);
            if (count == 1) {
                builder.append(std::to_string(dateTime.getDays()));// GCOVR_EXCL_LINE
            } else if (count == 2) {
                if (dateTime.getDays() < 10) {
                    builder.append('0');// GCOVR_EXCL_LINE
                }
                builder.append(std::to_string(dateTime.getDays()));// GCOVR_EXCL_LINE
            } else if (count == 3) {
                builder.append(WkDay[dateTime.getDayOfWeek()]);// GCOVR_EXCL_LINE
            } else {
                builder.append(WeekDay[dateTime.getDayOfWeek()]);// GCOVR_EXCL_LINE
            }
            input.trunc(count);// GCOVR_EXCL_LINE
        } else if (buffer[0] == 'm') {
            auto count = ::count('m', buffer);
            if (count == 1) {
                builder.append(std::to_string(dateTime.getMinutes()));// GCOVR_EXCL_LINE
                input.trunc(1);                                       // GCOVR_EXCL_LINE
            } else if (count > 1) {
                if (dateTime.getMinutes() < 10) {
                    builder.append('0');
                }
                builder.append(std::to_string(dateTime.getMinutes()));// GCOVR_EXCL_LINE
                input.trunc(2);                                       // GCOVR_EXCL_LINE
            }
        } else if (buffer[0] == 'M') {
            auto count = ::count('M', buffer);
            if (count == 1) {
                builder.append(std::to_string(dateTime.getMonths() + 1));// GCOVR_EXCL_LINE
            } else if (count == 2) {
                if (dateTime.getMinutes() < 10) {
                    builder.append('0');// GCOVR_EXCL_LINE
                }
                builder.append(std::to_string(dateTime.getMonths() + 1));// GCOVR_EXCL_LINE
            } else if (count == 3) {
                builder.append(Mon[dateTime.getMonths()]);// GCOVR_EXCL_LINE
            } else {
                builder.append(Month[dateTime.getMonths()]);// GCOVR_EXCL_LINE
            }
            input.trunc(count);// GCOVR_EXCL_LINE
        } else if (buffer[0] == 'y') {
            auto count = ::count('y', buffer);
            if (count == 1) {
                builder.append(std::to_string(dateTime.getYears() % 100));// GCOVR_EXCL_LINE
            } else if (count == 2) {
                auto year = dateTime.getYears() % 100;
                if (year < 10) {
                    builder.append('0');// GCOVR_EXCL_LINE
                }
                builder.append(std::to_string(year));// GCOVR_EXCL_LINE
            } else if (count == 3) {
                auto year = dateTime.getYears() % 1000;
                if (year < 100) {
                    builder.append('0');// GCOVR_EXCL_LINE
                }
                if (year < 10) {
                    builder.append('0');// GCOVR_EXCL_LINE
                }
                builder.append(std::to_string(year));// GCOVR_EXCL_LINE
            } else {
                // 此处时间通常不会小于 10，因为 DateTime 时间戳默认是无符号整数
                // GCOVR_EXCL_START
                auto year = dateTime.getYears() % 10000;
                if (year < 1000) {
                    builder.append('0');
                }
                if (year < 100) {
                    builder.append('0');
                }
                if (year < 10) {
                    builder.append('0');
                }
                // GCOVR_EXCL_STOP
                builder.append(std::to_string(year));// GCOVR_EXCL_LINE
            }
            input.trunc(count);// GCOVR_EXCL_LINE
        } else if (buffer[0] == 'z') {
            if (dateTime.getUTC() >= 0) {
                builder.append('+');// GCOVR_EXCL_LINE
            }
            builder.append(std::to_string(dateTime.getUTC()));// GCOVR_EXCL_LINE
            input.trunc(1);                                   // GCOVR_EXCL_LINE
        } else if (buffer[0] == 'h') {
            auto count = ::count('h', buffer);
            auto hour = dateTime.getHours();
            hour = hour % 12 == 0 ? 12 : hour % 12;
            if (count == 1) {
                builder.append(std::to_string(hour));// GCOVR_EXCL_LINE
                input.trunc(1);                      // GCOVR_EXCL_LINE
            } else if (count > 1) {
                if (hour < 10) {
                    builder.append('0');// GCOVR_EXCL_LINE
                }
                builder.append(std::to_string(hour));// GCOVR_EXCL_LINE
                input.trunc(2);                      // GCOVR_EXCL_LINE
            }
        } else if (buffer[0] == 'H') {
            auto count = ::count('H', buffer);
            if (count == 1) {
                builder.append(std::to_string(dateTime.getHours()));// GCOVR_EXCL_LINE
                input.trunc(1);                                     // GCOVR_EXCL_LINE
            } else if (count > 1) {
                if (dateTime.getHours() < 10) {
                    builder.append('0');// GCOVR_EXCL_LINE
                }
                builder.append(std::to_string(dateTime.getHours()));// GCOVR_EXCL_LINE
                input.trunc(2);                                     // GCOVR_EXCL_LINE
            }
        } else if (buffer[0] == 't') {
            builder.append(dateTime.getHours() < 12 ? "AM" : "PM");// GCOVR_EXCL_LINE
            input.trunc(1);                                        // GCOVR_EXCL_LINE
        } else if (buffer[0] == 's') {
            auto count = ::count('s', buffer);
            if (count == 1) {
                builder.append(std::to_string(dateTime.getSeconds()));// GCOVR_EXCL_LINE
                input.trunc(1);                                       // GCOVR_EXCL_LINE
            } else if (count > 1) {
                if (dateTime.getSeconds() < 10) {
                    builder.append('0');// GCOVR_EXCL_LINE
                }
                builder.append(std::to_string(dateTime.getSeconds()));// GCOVR_EXCL_LINE
                input.trunc(2);                                       // GCOVR_EXCL_LINE
            }
        } else if (buffer[0] == 'f') {
            auto count = ::count('f', buffer);
            if (count == 1) {
                builder.append(std::to_string(dateTime.getMilliseconds()));// GCOVR_EXCL_LINE
            } else if (count == 2) {
                auto num = dateTime.getMilliseconds();
                if (num < 100) {
                    builder.append('0');// GCOVR_EXCL_LINE
                }
                if (num < 10) {
                    builder.append('0');// GCOVR_EXCL_LINE
                }
                builder.append(std::to_string(num));// GCOVR_EXCL_LINE
            } else if (count == 3) {
                builder.append(std::to_string(dateTime.getMicroseconds()));// GCOVR_EXCL_LINE
            } else {
                auto num = dateTime.getMicroseconds();
                if (num < 100) {
                    builder.append('0');// GCOVR_EXCL_LINE
                }
                if (num < 10) {
                    builder.append('0');// GCOVR_EXCL_LINE
                }
                builder.append(std::to_string(num));// GCOVR_EXCL_LINE
            }
            input.trunc(count);// GCOVR_EXCL_LINE
        } else if (buffer[0] == '%') {
            if (len > 1) {
                builder.append(buffer[1]);// GCOVR_EXCL_LINE
                input.trunc(2);           // GCOVR_EXCL_LINE
            } else {
                input.trunc(1);// GCOVR_EXCL_LINE
            }
        } else {
            builder.append(buffer[0]);// GCOVR_EXCL_LINE
            input.trunc(1);           // GCOVR_EXCL_LINE
        }
    }

    return builder.toString();// GCOVR_EXCL_LINE
}

std::string sese::text::DateTimeFormatter::format(const sese::DateTime::Ptr &dateTime, const std::string &pattern) {
    return format(*dateTime.get(), pattern);
}

#ifdef WIN32
#pragma warning(disable : 4996)
#endif

inline int mon2number(const char *str) {
    for (int i = 0; i < 12; i++) {
        if (strcasecmp(str, sese::text::DateTimeFormatter::Mon[i]) == 0) {
            return i;
        }
    }
    return -1;
}

#ifdef _timezone
#define timezone _timezone
#endif

// 此处不处理错误的格式
// GCOVR_EXCL_START

uint64_t sese::text::DateTimeFormatter::parseFromGreenwich(const std::string &text) {
    std::tm tm{};
    if (text[7] == '-') {
        // 另一种时间格式
        auto date = StringBuilder::split(text, " ");

        char _h[3]{0};
        char _m[3]{0};
        char _s[3]{0};
        memcpy(_h, date[2].c_str() + 0, 2);
        memcpy(_m, date[2].c_str() + 3, 2);
        memcpy(_s, date[2].c_str() + 6, 2);

        char *end;
        tm.tm_hour = (int) std::strtol(_h, &end, 10);
        tm.tm_min = (int) std::strtol(_m, &end, 10);
        tm.tm_sec = (int) std::strtol(_s, &end, 10);

        date = StringBuilder::split(date[1], "-");
        tm.tm_mday = std::stoi(date[0]);
        tm.tm_mon = mon2number(date[1].c_str());
        tm.tm_year = std::stoi(date[2]) + 100;
    } else {
        auto date = StringBuilder::split(text, " ");

        tm.tm_mday = std::stoi(date[1]);
        tm.tm_mon = mon2number(date[2].c_str());
        tm.tm_year = std::stoi(date[3]) - 1900;

        char _h[3]{0};
        char _m[3]{0};
        char _s[3]{0};
        memcpy(_h, date[4].c_str() + 0, 2);
        memcpy(_m, date[4].c_str() + 3, 2);
        memcpy(_s, date[4].c_str() + 6, 2);

        char *end;
        tm.tm_hour = (int) std::strtol(_h, &end, 10);
        tm.tm_min = (int) std::strtol(_m, &end, 10);
        tm.tm_sec = (int) std::strtol(_s, &end, 10);
    }
    return std::mktime(&tm) - timezone;
}

uint64_t sese::text::DateTimeFormatter::parseFromISO8601(const std::string &text) {
    std::tm tm{};
    if (text.length() == 10) {
        auto date = StringBuilder::split(text, "-");
        if (date.size() == 3) {
            tm.tm_year = (int) (std::stoi(date[0]) - 1900);
            tm.tm_mon = (int) (std::stoi(date[1]) - 1);
            tm.tm_mday = (int) std::stoi(date[2]);
            return std::mktime(&tm) - timezone;
        } else {
            return UINT64_MAX;
        }
    } else if (text.length() == 19) {
        auto dateTime = StringBuilder::split(text, " ");
        if (dateTime.size() != 2) return UINT64_MAX;

        auto date = StringBuilder::split(dateTime[0], "-");
        if (date.size() == 3) {
            tm.tm_year = (int) (std::stoi(date[0]) - 1900);
            tm.tm_mon = (int) (std::stoi(date[1]) - 1);
            tm.tm_mday = (int) (std::stoi(date[2]));
        } else {
            return UINT64_MAX;
        }

        auto time = StringBuilder::split(dateTime[1], ":");
        if (time.size() == 3) {
            tm.tm_hour = (int) std::stoi(time[0]);
            tm.tm_min = (int) std::stoi(time[1]);
            tm.tm_sec = (int) std::stoi(time[2]);
            return std::mktime(&tm) - timezone;
        } else {
            return UINT64_MAX;
        }
    } else if (text.length() == 20) {
        auto dateTime = StringBuilder::split(text, "T");
        if (dateTime.size() != 2) return UINT64_MAX;

        auto date = StringBuilder::split(dateTime[0], "-");
        if (date.size() == 3) {
            tm.tm_year = (int) (std::stoi(date[0]) - 1900);
            tm.tm_mon = (int) (std::stoi(date[1]) - 1);
            tm.tm_mday = (int) (std::stoi(date[2]));
        } else {
            return UINT64_MAX;
        }

        auto time = StringBuilder::split(dateTime[1], ":");
        if (time.size() == 3) {
            tm.tm_hour = (int) std::stoi(time[0]);
            tm.tm_min = (int) std::stoi(time[1]);
            tm.tm_sec = (int) std::stoi(time[2]);
            return std::mktime(&tm) - timezone;
        } else {
            return UINT64_MAX;
        }
    } else if (text.length() == 22) {
        auto dateTimeZone = StringBuilder::split(text, " ");
        if (dateTimeZone.size() != 3) return UINT64_MAX;

        auto date = StringBuilder::split(dateTimeZone[0], "-");
        if (date.size() == 3) {
            tm.tm_year = (int) (std::stoi(date[0]) - 1900);
            tm.tm_mon = (int) (std::stoi(date[1]) - 1);
            tm.tm_mday = (int) (std::stoi(date[2]));
        } else {
            return UINT64_MAX;
        }

        auto time = StringBuilder::split(dateTimeZone[1], ":");
        if (time.size() == 3) {
            tm.tm_hour = (int) std::stoi(time[0]);
            tm.tm_min = (int) std::stoi(time[1]);
            tm.tm_sec = (int) std::stoi(time[2]);
            return std::mktime(&tm) - std::stoi(dateTimeZone[2]) * 60 * 60 - timezone;
        } else {
            return UINT64_MAX;
        }
    } else if (text.length() == 25) {
        auto dateTimeZone = StringBuilder::split(text, "T");
        if (dateTimeZone.size() != 2) return UINT64_MAX;

        auto date = StringBuilder::split(dateTimeZone[0], "-");
        if (date.size() == 3) {
            tm.tm_year = (int) (std::stoi(date[0]) - 1900);
            tm.tm_mon = (int) (std::stoi(date[1]) - 1);
            tm.tm_mday = (int) (std::stoi(date[2]));
        } else {
            return UINT64_MAX;
        }

        auto time = StringBuilder::split(dateTimeZone[1], ":");
        if (time.size() == 4) {
            tm.tm_hour = (int) std::stoi(time[0]);
            tm.tm_min = (int) std::stoi(time[1]);
            tm.tm_sec = (int) std::stoi(time[2]);
        } else {
            return UINT64_MAX;
        }

        auto timeZone = StringBuilder::split(text, "+");
        if (timeZone.size() != 2) return UINT64_MAX;
        auto zone = StringBuilder::split(timeZone[1], ":");
        if (zone.size() != 2) return UINT64_MAX;
        auto hour = std::stoi(zone[0]);
        auto min = std::stoi(zone[1]);
        return std::mktime(&tm) - (((hour * 60) + min) * 60) - timezone;
    } else {
        return UINT64_MAX;
    }
}

// GCOVR_EXCL_STOP