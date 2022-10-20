#include "sese/text/DateTimeFormatter.h"
#include <cstring>
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

const char *Month[] = {"January",
                       "February",
                       "March",
                       "April",
                       "May",
                       "June",
                       "July",
                       "August",
                       "September",
                       "October",
                       "November",
                       "December"};

const char *Mon[] = {"Jan",
                     "Feb",
                     "Mar",
                     "Apr",
                     "May",
                     "Jun",
                     "Jul",
                     "Aug",
                     "Sep",
                     "Oct",
                     "Nov",
                     "Dec"};

const char *WeekDay[] = {"Sunday",
                         "Monday",
                         "Tuesday",
                         "Wednesday",
                         "Thursday",
                         "Friday",
                         "Saturday"};

const char *WkDay[] = {"Sun",
                       "MonDays",
                       "Tue",
                       "Wed",
                       "Thu",
                       "Fri",
                       "Sat"};


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
