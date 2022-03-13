#include "SimpleFormatter.h"
#include "util/DateTimeFormatter.h"
#include <sstream>

#ifdef _WIN32
#pragma warning(disable : 4018)
#pragma warning(disable : 4996)
#endif

namespace sese {

    extern "C" API const char *getLevelString(Level level) noexcept {
        switch (level) {
            case Level::DEBUG:
                return "DEBUG";
            case Level::INFO:
                return "INFO ";
            case Level::WARN:
                return "WARN ";
            case Level::ERR:
                return "ERROR";
            default:
                return "DEBUG ";
        }
    }

    SimpleFormatter::SimpleFormatter(const std::string &textPattern, const std::string &timePattern) noexcept : Formatter() {
        this->textPattern = textPattern;
        this->timePattern = timePattern;
    }

    std::string SimpleFormatter::dump(const Event::Ptr &event) noexcept {
        std::stringstream stream;
        auto len = this->textPattern.length();
        for (auto i = 0; i < len; i++) {
            if (textPattern[i] == '%') {
                if (i <= len - 2) {
                    switch (textPattern[i + 1]) {
                        case '%':
                            stream << '%';
                            i += 1;
                            break;
                        case 'c': {
                            auto time = event->getTime();
                            auto rt = DateTimeFormatter::format(time, this->timePattern);
                            stream << rt;
                            i += 1;
                            break;
                        }
                        case 'm':
                            stream << event->getMessage();
                            i += 1;
                            break;
                        case 'l':
                            if (textPattern[i + 2] == 'i') {
                                stream << event->getLine();
                                i += 2;
                            } else if (textPattern[i + 2] == 'v') {
                                stream << getLevelString(event->getLevel());
                                i += 2;
                            } else {
                                i += 1;
                            }
                            break;
                        case 'f':
                            stream << event->getFileName();
                            i += 1;
                            break;
                        case 't':
                            if (textPattern[i + 2] == 'h') {
                                stream << event->getThreadId();
                                i += 2;
                            } else if (textPattern[i + 2] == 'n') {
                                stream << event->getThreadName();
                                i += 2;
                            } else {
                                i += 1;
                            }
                            break;
                        default:
                            break;
                    }
                } else {
                    break;
                }
            } else {
                stream << textPattern[i];
            }
        }
        return stream.str();
    }

}// namespace sese