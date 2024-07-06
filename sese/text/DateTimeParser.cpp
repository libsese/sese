#include <sese/Config.h>
#include <sese/text/DateTimeParser.h>
#include <sese/io/InputBufferWrapper.h>

#include <chrono>

using sese::DateTime;
using sese::text::DateTimeParser;

int DateTimeParser::count(const char *string, int max) {
    char first_char = *string;
    const char *p = string;
    int count = 0;

    while (p[count] && count <= max) {
        if (p[count] == first_char) {
            count += 1;
        } else {
            break;
        }
    }

    return count;
}

std::optional<DateTime> DateTimeParser::parse(const std::string &format, const std::string &datetime) {
    if (format.empty() || datetime.empty())
        return std::nullopt;

    const char *p_format = format.c_str();
    auto input = sese::io::InputBufferWrapper(datetime.c_str(), datetime.length());

    int year = 0;
    int mon = 0;
    int day = 0;
    int hour = 0;
    int min = 0;
    int sec = 0;
    int mill = 0;
    int micro = 0;
    int utc = 0;

#define XX(ch, exp_size, save_to)                           \
    case ch: {                                              \
        char *end;                                          \
        auto c = count(p_format, exp_size);                 \
        if (c >= exp_size) {                                \
            char buffer[exp_size + 1]{};                    \
            if (input.read(buffer, exp_size) != exp_size) { \
                return std::nullopt;                        \
            }                                               \
            save_to = std::strtol(buffer, &end, 10);        \
            if (*end != 0) {                                \
                return std::nullopt;                        \
            }                                               \
            p_format += exp_size;                           \
        } else {                                            \
            p_format += c;                                  \
        }                                                   \
        break;                                              \
    }

    while (*p_format) {
        switch (*p_format) {
            XX('y', 4, year)
            XX('M', 2, mon)
            XX('d', 2, day)
            XX('H', 2, hour)
            XX('m', 2, min)
            XX('s', 2, sec)
            XX('f', 3, mill)
            XX('r', 3, micro)
            case 'z': {
                char *end;
                char buffer[4]{};
                if (input.read(buffer, 2) != 2) {
                    break;
                }
                if (buffer[0] == '+' || buffer[0] == '-') {
                    if (input.read(&buffer[2], 1) != 1) {
                        break;
                    }
                }
                utc = static_cast<int>(std::strtol(buffer, &end, 10));
                if (*end != 0) {
                    return std::nullopt;
                }
                p_format += 1;
                break;
            }
            case '*': {
                while (true) {
                    char c{};
                    auto l = input.peek(&c, 1);
                    if (l == 0 && p_format + 1 <= format.c_str() + format.length()) {
                        break;
                    } else if (c == ' ') {
                        p_format += 1;
                        break;
                    } else {
                        input.trunc(1);
                    }
                }
                break;
            }
            case '%': {
                if (p_format + 1 > format.c_str() + format.length()) {
                    break;
                } else {
                    p_format += 2;
                    break;
                }
            }
            default:
                p_format += 1;
                input.trunc(1);
                break;
        }
    }
#undef XX

    struct tm tm = {};
    tm.tm_year = year - 1900;
    tm.tm_mon = mon - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = min;
    tm.tm_sec = sec;

    auto tp = std::chrono::system_clock::from_time_t(timegm(&tm));
    tp += std::chrono::milliseconds(mill) + std::chrono::microseconds(micro);
    tp -= std::chrono::hours(utc);

    // Convert time_point to microseconds
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::microseconds>(tp.time_since_epoch()).count();

    return DateTime(timestamp, utc, DateTime::Policy::FORMAT);
}