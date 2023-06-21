#include "sese/text/DateTimeFormatter.h"
#include "sese/text/StringBuilder.h"

#include <cstring>
#include <ctime>

#ifdef _WIN32
#define itoa _itoa_s
#else
static char *itoa(int num, char *str, int radix = 10) {
    char index[] = "0123456789ABCDEF";
    unsigned unum;
    int i = 0, j, k;
    if (radix == 10 && num < 0) {
        unum = (unsigned) -num;
        str[i++] = '-';
    } else
        unum = (unsigned) num;
    do {
        str[i++] = index[unum % (unsigned) radix];
        unum /= radix;
    } while (unum);
    str[i] = '\0';
    if (str[0] == '-')
        k = 1;
    else
        k = 0;

    for (j = k; j <= (i - 1) / 2; j++) {
        char temp;
        temp = str[j];
        str[j] = str[i - 1 + k - j];
        str[i - 1 + k - j] = temp;
    }
    return str;
}
#endif

const char *Month[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

const char *Mon[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

const char *WeekDay[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

const char *WkDay[] = {"Sun", "MonDays", "Tue", "Wed", "Thu", "Fri", "Sat"};


static int count(const char &ch, const char *str) {
    for (int i = 0; i < 4; i++) {
        if (*(str + i) != ch) {
            return i;
        }
    }
    return 4;
}

std::string sese::text::DateTimeFormatter::format(const sese::DateTime &dateTime, const std::string &pattern) {
    auto builder = std::make_shared<StringBuilder>(128);
    int c;
    char buffer[5];
    for (int i = 0; i < pattern.length();) {
        switch (pattern[i]) {
            case 'd': {
                c = count('d', &pattern[i]);
                itoa(dateTime.getDays(), buffer, 10);
                if (c == 1) {
                    builder->append(buffer);
                } else if (c == 2) {
                    if (dateTime.getDays() < 10) {
                        builder->append('0');
                    }
                    builder->append(buffer);
                } else if (c == 3) {
                    builder->append(WkDay[dateTime.getDayOfWeek()]);
                } else if (c == 4) {
                    builder->append(WeekDay[dateTime.getDayOfWeek()]);
                }
                i += c;
                memset(buffer, 0, 4);
                break;
            }
            case 'm': {
                c = count('m', &pattern[i]);
                itoa(dateTime.getMinutes(), buffer, 10);
                if (c == 1) {
                    builder->append(buffer);
                } else if (c == 2) {
                    if (dateTime.getMinutes() < 10) {
                        builder->append('0');
                    }
                    builder->append(buffer);
                }
                i += c;
                memset(buffer, 0, 4);
                break;
            }
            case 'M': {
                c = count('M', &pattern[i]);
                itoa(dateTime.getMonths(), buffer, 10);
                if (c == 1) {
                    builder->append(buffer);
                } else if (c == 2) {
                    if (dateTime.getMonths() < 10) {
                        builder->append('0');
                    }
                    builder->append(buffer);
                } else if (c == 3) {
                    builder->append(Mon[dateTime.getMonths()]);
                } else {
                    builder->append(Month[dateTime.getMonths()]);
                }
                i += c;
                memset(buffer, 0, 4);
                break;
            }
            case 'y': {
                c = count('y', &pattern[i]);
                if (c == 1) {
                    itoa(dateTime.getYears() % 100, buffer, 10);
                    builder->append(buffer);
                } else if (c == 2) {
                    if (dateTime.getYears() < 10) {
                        builder->append('0');
                    }
                    itoa(dateTime.getYears() % 100, buffer, 10);
                    builder->append(buffer);
                } else if (c == 3) {
                    if (dateTime.getYears() < 10) {
                        builder->append("00");
                    } else if (dateTime.getYears() < 100) {
                        builder->append('0');
                    }
                    itoa(dateTime.getYears() % 1000, buffer, 10);
                    builder->append(buffer);
                } else if (c == 4) {
                    if (dateTime.getYears() < 10) {
                        builder->append("000");
                    } else if (dateTime.getYears() < 100) {
                        builder->append("00");
                    } else if (dateTime.getYears() < 1000) {
                        builder->append('0');
                    }
                    itoa(dateTime.getYears(), buffer, 10);
                    builder->append(buffer);
                }
                i += c;
                memset(buffer, 0, 4);
                break;
            }
            case 'z': {
                if (dateTime.getUTC() >= 0) {
                    builder->append('+');
                }
                itoa(dateTime.getUTC(), buffer, 10);
                builder->append(buffer);
                memset(buffer, 0, 4);
                i += 1;
                break;
            }
            case 'h': {
                c = count('h', &pattern[i]);
                if (c == 1) {
                    itoa(dateTime.getHours() % 12, buffer, 10);
                    builder->append(buffer);
                } else if (c == 2) {
                    if (dateTime.getHours() % 12 < 10) {
                        builder->append('0');
                    }
                    itoa(dateTime.getHours() % 12, buffer, 10);
                    builder->append(buffer);
                }
                memset(buffer, 0, 4);
                i += 1;
                break;
            }
            case 'H': {
                c = count('H', &pattern[i]);
                if (c == 1) {
                    itoa(dateTime.getHours(), buffer, 10);
                    builder->append(buffer);
                } else if (c == 2) {
                    if (dateTime.getHours() < 10) {
                        builder->append('0');
                    }
                    itoa(dateTime.getHours(), buffer, 10);
                    builder->append(buffer);
                }
                memset(buffer, 0, 4);
                i += c;
                break;
            }
            case 's': {
                c = count('s', &pattern[i]);
                if (c == 1) {
                    itoa(dateTime.getSeconds(), buffer, 10);
                    builder->append(buffer);
                } else if (c == 2) {
                    if (dateTime.getSeconds() < 10) {
                        builder->append('0');
                    }
                    itoa(dateTime.getSeconds(), buffer, 10);
                    builder->append(buffer);
                }
                memset(buffer, 0, 4);
                i += c;
                break;
            }
            case 'f': {
                c = count('f', &pattern[i]);
                if (c == 1) {
                    itoa(dateTime.getMilliseconds(), buffer, 10);
                    builder->append(buffer);
                } else if (c == 2) {
                    if (dateTime.getMilliseconds() < 10) {
                        builder->append("00");
                    } else if (dateTime.getMilliseconds() < 100) {
                        builder->append('0');
                    }
                    itoa(dateTime.getMilliseconds(), buffer, 10);
                    builder->append(buffer);
                } else if (c == 3) {
                    itoa(dateTime.getMicroseconds(), buffer, 10);
                    builder->append(buffer);
                } else if (c == 4) {
                    if (dateTime.getMicroseconds() < 10) {
                        builder->append("00");
                    } else if (dateTime.getMicroseconds() < 100) {
                        builder->append('0');
                    }
                    itoa(dateTime.getMicroseconds(), buffer, 10);
                    builder->append(buffer);
                }
                i += c;
                memset(buffer, 0, 4);
                break;
            }
            case 't': {
                if (dateTime.getHours() / 12 == 0) {
                    builder->append("AM");
                } else {
                    builder->append("PM");
                }
                i += 1;
            }
            case '%': {
                if (pattern.length() - 1 >= i) {
                    builder->append(pattern[i + 1]);
                    i += 2;
                } else {
                    builder->append('%');
                    i += 1;
                }
                break;
            }
            default: {
                builder->append(pattern[i]);
                i += 1;
                break;
            }
        }
    }
    return builder->toString();
}

std::string sese::text::DateTimeFormatter::format(const sese::DateTime::Ptr &dateTime, const std::string &pattern) {
    return format(*dateTime.get(), pattern);
}

#ifdef WIN32
#pragma warning(disable : 4996)
#endif

inline int mon2number(const char *str) {
    for (int i = 0; i < 12; i++) {
        if (strcasecmp(str, Mon[i]) == 0) {
            return i;
        }
    }
    return -1;
}

#ifdef _timezone
#define timezone _timezone
#endif

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
        tm.tm_year = std::stoi(date[2]);
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