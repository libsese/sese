#include "sese/record/LogHelper.h"
#include "sese/thread/Thread.h"
#include "sese/Util.h"
#include <cstdarg>

using sese::record::LogHelper;

LogHelper::LogHelper(const char *tag) {
    this->filter = tag;
    logger = record::getLogger();
}

void LogHelper::log(record::Level level, const char *file, int32_t line, const char *format, va_list ap) {
    char buf[RECORD_OUTPUT_BUFFER]{0};
    vsprintf(buf, format, ap);
    record::Event::Ptr event = std::make_shared<record::Event>(sese::DateTime::now(), level, sese::Thread::getCurrentThreadName(), sese::Thread::getCurrentThreadId(), file, line, buf, filter);
    logger->log(event);
}

void LogHelper::debug(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    log(record::Level::DEBUG, __FILE__, __LINE__, format, ap);
    va_end(ap);
}

void LogHelper::info(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    log(record::Level::INFO, __FILE__, __LINE__, format, ap);
    va_end(ap);
}

void LogHelper::warn(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    log(record::Level::WARN, __FILE__, __LINE__, format, ap);
    va_end(ap);
}

void LogHelper::error(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    log(record::Level::ERR, __FILE__, __LINE__, format, ap);
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
