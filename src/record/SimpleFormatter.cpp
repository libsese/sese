#include "SimpleFormatter.h"
#include "Util.h"
#include <sstream>

#ifdef _WIN32
#pragma warning(disable : 4018)
#pragma warning(disable : 4996)
#endif

namespace sese {

    SimpleFormatter::SimpleFormatter(const std::string &pattern) noexcept {
        this->pattern = pattern;
    }

    std::string SimpleFormatter::dump(const Event::Ptr &event) noexcept {
        std::stringstream stream;
        auto len = this->pattern.length();
        for (auto i = 0; i < len; i++) {
            if (pattern[i] == '%') {
                if (i <= len - 2) {
                    switch (pattern[i + 1]) {
                        case '%':
                            stream << '%';
                            i += 1;
                            break;
                        case 'c': {
                            auto time = event->getTime();
                            auto info = localtime(reinterpret_cast<const time_t *>(&time));
                            char buffer[64];
                            strftime(buffer, 64, "%x %X", info);
                            stream << buffer;
                            i += 1;
                            break;
                        }
                        case 'm':
                            stream << event->getMessage();
                            i += 1;
                            break;
                        case 'l':
                            if (pattern[i + 2] == 'i') {
                                stream << event->getLine();
                                i += 2;
                            } else if (pattern[i + 2] == 'v') {
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
                            if (pattern[i + 2] == 'h') {
                                stream << event->getThreadId();
                                i += 2;
                            } else if (pattern[i + 2] == 'n') {
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
                stream << pattern[i];
            }
        }
        return stream.str();
    }

}// namespace sese