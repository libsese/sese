// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
    const auto EVENT = std::make_shared<record::Event>(sese::DateTime::now(), level, sese::Thread::getCurrentThreadName(), sese::Thread::getCurrentThreadId(), file, line, buf);
    getLogger()->log(EVENT);
}

LogHelper::LogHelper() {
    logger = record::getLogger();
}

void LogHelper::log(record::Level level, const char *file, int32_t line, const char *format, va_list ap) {
    char buf[RECORD_OUTPUT_BUFFER]{0};
    std::vsnprintf(buf, RECORD_OUTPUT_BUFFER,format, ap);
    const auto EVENT = std::make_shared<record::Event>(sese::DateTime::now(), level, sese::Thread::getCurrentThreadName(), sese::Thread::getCurrentThreadId(), file, line, buf);
    logger->log(EVENT);
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
