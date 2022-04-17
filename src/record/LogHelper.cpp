#include "record/LogHelper.h"
#include "Util.h"
#include <cstdarg>

sese::LogHelper::LogHelper(const char *filter) {
    this->filter = filter;
    logger = sese::getLogger();
}

void sese::LogHelper::log(Level level, const char *format, ...) {
    char buf[RECORD_OUTPUT_BUFFER];
    va_list ap;
    va_start(ap, format);
    sprintf(buf, format, ap);
    va_end(ap);
    sese::Event::Ptr event = std::make_shared<sese::Event>(sese::DateTime::now(), level, sese::Thread::getCurrentThreadName().c_str(), sese::Thread::getCurrentThreadId(), FN, __LINE__, buf, filter);
    logger->log(event);
}

void sese::LogHelper::debug(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    log(sese::Level::DEBUG, format, ap);
    va_end(ap);
}

void sese::LogHelper::info(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    log(sese::Level::INFO, format, ap);
    va_end(ap);
}

void sese::LogHelper::warn(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    log(sese::Level::WARN, format, ap);
    va_end(ap);
}

void sese::LogHelper::error(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    log(sese::Level::ERR, format, ap);
    va_end(ap);
}