#include "sese/record/SimpleFormatter.h"
#include "sese/text/DateTimeFormatter.h"
#include <sstream>

#ifdef _WIN32
#pragma warning(disable : 4018)
#pragma warning(disable : 4996)
#endif

namespace sese {

    extern "C" API const char *getLevelString(record::Level level) noexcept {
        switch (level) {
            case record::Level::DEBUG:
                return "DEBUG";
            case record::Level::INFO:
                return "INFO ";
            case record::Level::WARN:
                return "WARN ";
            case record::Level::ERR:
                return "ERROR";
            default:
                return "DEBUG";
        }
    }

    record::SimpleFormatter::SimpleFormatter(const std::string &textPattern, const std::string &timePattern) noexcept
            : AbstractFormatter() {
        this->textPattern = textPattern;
        this->timePattern = timePattern;
    }

    std::string record::SimpleFormatter::dump(const Event::Ptr &event) noexcept {
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
                            decltype(auto) time = event->getTime();
                            auto rt = text::DateTimeFormatter::format(time, this->timePattern);
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
                            if (textPattern[i + 2] == 'i') {
                                stream << event->getTag();
                                i += 2;
                            } else if (textPattern[i + 2] == 'n') {
                                stream << event->getFileName();
                                i += 2;
                            } else {
                                i += 1;
                            }
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