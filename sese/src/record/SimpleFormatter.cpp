#include "sese/record/SimpleFormatter.h"
#include "sese/text/DateTimeFormatter.h"
#include "sese/text/StringBuilder.h"
#include "sese/util/InputBufferWrapper.h"

#ifdef _WIN32
#pragma warning(disable : 4018)
#pragma warning(disable : 4996)
#endif

namespace sese {

    extern "C" API const char *getLevelString(record::Level level) noexcept {
        switch (level) {// GCOVR_EXCL_LINE
            case record::Level::DEBUG:
                return "D";
            case record::Level::INFO:
                return "I";
            case record::Level::WARN:
                return "W";
            case record::Level::ERR:
                return "E";
            default:           // GCOVR_EXCL_LINE
                return "DEBUG";// GCOVR_EXCL_LINE
        }
    }

    record::SimpleFormatter::SimpleFormatter(const std::string &textPattern, const std::string &timePattern) noexcept
        : AbstractFormatter() {
        this->textPattern = textPattern;
        this->timePattern = timePattern;
    }

    std::string record::SimpleFormatter::dump(const Event::Ptr &event) noexcept {
        sese::text::StringBuilder builder(1024);
        auto input = sese::InputBufferWrapper(textPattern.c_str(), textPattern.length());
        while (true) {
            char buf[2]{};
            auto len = input.peek(buf, 2);// GCOVR_EXCL_LINE
            if (len == 0) {
                break;
            } else {
                if (buf[0] == 'c') {
                    builder.append(sese::text::DateTimeFormatter::format(event->getTime(), this->timePattern));// GCOVR_EXCL_LINE
                    input.trunc(1);                                                                            // GCOVR_EXCL_LINE
                    continue;
                } else if (buf[0] == 'm') {
                    builder.append(event->getMessage());// GCOVR_EXCL_LINE
                    input.trunc(1);                     // GCOVR_EXCL_LINE
                    continue;
                } else if (len != 2) {
                    builder.append(buf[0]);// GCOVR_EXCL_LINE
                    input.trunc(1);        // GCOVR_EXCL_LINE
                    continue;
                }

                if (buf[0] == 'l') {
                    if (buf[1] == 'i') {
                        builder.append(std::to_string(event->getLine()));// GCOVR_EXCL_LINE
                        input.trunc(2);                                  // GCOVR_EXCL_LINE
                    } else if (buf[1] == 'v') {
                        builder.append(getLevelString(event->getLevel()));// GCOVR_EXCL_LINE
                        input.trunc(2);                                   // GCOVR_EXCL_LINE
                    } else {
                        builder.append(buf[0]);// GCOVR_EXCL_LINE
                        input.trunc(1);        // GCOVR_EXCL_LINE
                    }
                } else if (buf[0] == 'f') {
                    if (buf[1] == 'n') {
                        builder.append(event->getFileName());// GCOVR_EXCL_LINE
                        input.trunc(2);                      // GCOVR_EXCL_LINE
                    } else {
                        builder.append(buf[0]);// GCOVR_EXCL_LINE
                        input.trunc(1);        // GCOVR_EXCL_LINE
                    }
                } else if (buf[0] == 't') {
                    if (buf[1] == 'h') {
                        builder.append(std::to_string(event->getThreadId()));// GCOVR_EXCL_LINE
                        input.trunc(2);                                      // GCOVR_EXCL_LINE
                    } else if (buf[1] == 'n') {
                        builder.append(event->getThreadName());// GCOVR_EXCL_LINE
                        input.trunc(2);                        // GCOVR_EXCL_LINE
                    } else {
                        builder.append(buf[0]);// GCOVR_EXCL_LINE
                        input.trunc(1);        // GCOVR_EXCL_LINE
                    }
                } else if (buf[0] == '%') {
                    builder.append(buf[1]);// GCOVR_EXCL_LINE
                    input.trunc(2);        // GCOVR_EXCL_LINE
                } else {
                    builder.append(buf[0]);// GCOVR_EXCL_LINE
                    input.trunc(1);        // GCOVR_EXCL_LINE
                }
            }
        }
        return builder.toString();
    }

}// namespace sese