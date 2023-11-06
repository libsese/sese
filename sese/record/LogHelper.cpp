#include "sese/record/LogHelper.h"
#include "sese/thread/Thread.h"
#include "sese/util/Util.h"

#include <cstdarg>

using sese::record::LogHelper;

void LogHelper::d(const char *format, ...) noexcept {
    va_list ap;
    va_start(ap, format);
    l(record::Level::DEBUG, SESE_FILENAME, __LINE__, format, ap); // GCOVR_EXCL_LINE
    va_end(ap);
}

void LogHelper::i(const char *format, ...) noexcept {
    va_list ap;
    va_start(ap, format);
    l(record::Level::INFO, SESE_FILENAME, __LINE__, format, ap); // GCOVR_EXCL_LINE
    va_end(ap);
}

void LogHelper::w(const char *format, ...) noexcept {
    va_list ap;
    va_start(ap, format);
    l(record::Level::WARN, SESE_FILENAME, __LINE__, format, ap); // GCOVR_EXCL_LINE
    va_end(ap);
}

void LogHelper::e(const char *format, ...) noexcept {
    va_list ap;
    va_start(ap, format);
    l(record::Level::ERR, SESE_FILENAME, __LINE__, format, ap); // GCOVR_EXCL_LINE
    va_end(ap);
}

void LogHelper::l(Level level, const char *file, int32_t line, const char *format, va_list ap) {
    char buf[RECORD_OUTPUT_BUFFER]{0};
    std::vsnprintf(buf, RECORD_OUTPUT_BUFFER,format, ap);
    record::Event::Ptr event = std::make_shared<record::Event>(sese::DateTime::now(), level, sese::Thread::getCurrentThreadName(), sese::Thread::getCurrentThreadId(), file, line, buf);
    getLogger()->log(event);
}

LogHelper::LogHelper() {
    logger = record::getLogger();
}

void LogHelper::log(record::Level level, const char *file, int32_t line, const char *format, va_list ap) {
    char buf[RECORD_OUTPUT_BUFFER]{0};
    std::vsnprintf(buf, RECORD_OUTPUT_BUFFER,format, ap);
    record::Event::Ptr event = std::make_shared<record::Event>(sese::DateTime::now(), level, sese::Thread::getCurrentThreadName(), sese::Thread::getCurrentThreadId(), file, line, buf);
    logger->log(event);
}

void LogHelper::debug(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    log(record::Level::DEBUG, SESE_FILENAME, __LINE__, format, ap); // GCOVR_EXCL_LINE
    va_end(ap);
}

void LogHelper::info(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    log(record::Level::INFO, SESE_FILENAME, __LINE__, format, ap); // GCOVR_EXCL_LINE
    va_end(ap);
}

void LogHelper::warn(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    log(record::Level::WARN, SESE_FILENAME, __LINE__, format, ap); // GCOVR_EXCL_LINE
    va_end(ap);
}

void LogHelper::error(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    log(record::Level::ERR, SESE_FILENAME, __LINE__, format, ap); // GCOVR_EXCL_LINE
    va_end(ap);
}

//void LogHelper::debug(const char *file, int32_t line, const char *format, ...) {
//    va_list ap;
//    va_start(ap, format);
//    log(sese::Level::DEBUG, file, line, format, ap);
//    va_end(ap);
//}
//
//void LogHelper::info(const char *file, int32_t line, const char *format, ...) {
//    va_list ap;
//    va_start(ap, format);
//    log(sese::Level::INFO, file, line, format, ap);
//    va_end(ap);
//}
//
//void LogHelper::warn(const char *file, int32_t line, const char *format, ...) {
//    va_list ap;
//    va_start(ap, format);
//    log(sese::Level::WARN, file, line, format, ap);
//    va_end(ap);
//}
//
//void sese::LogHelper::error(const char *file, int32_t line, const char *format, ...) {
//    va_list ap;
//    va_start(ap, format);
//    log(sese::Level::ERR, file, line, format, ap);
//    va_end(ap);
//}
