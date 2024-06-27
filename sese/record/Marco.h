/**
* @file Marco.h
* @author kaoru
* @version 0.1
* @brief 日志宏
* @date 2023年9月13日
*/

#pragma once

#include <sese/record/Logger.h>
#include <sese/text/Format.h>
#include <sese/text/Util.h>
#include <sese/thread/Thread.h>

#ifdef SESE_C_LIKE_FORMAT
#define __SESE_LOG(point_to_logger, level, format, ...)                                  \
    {                                                                                    \
        char sese_tmp_buf[RECORD_OUTPUT_BUFFER]{0};                                      \
        sese::text::snprintf(sese_tmp_buf, RECORD_OUTPUT_BUFFER, format, ##__VA_ARGS__); \
        auto sese_tmp_time = sese::DateTime::now();                                      \
        auto sese_tmp_event = std::make_shared<sese::record::Event>(                     \
                sese_tmp_time,                                                           \
                level,                                                                   \
                sese::Thread::getCurrentThreadName(),                                    \
                sese::Thread::getCurrentThreadId(),                                      \
                SESE_FILENAME,                                                           \
                __LINE__,                                                                \
                sese_tmp_buf                                                             \
        );                                                                               \
        point_to_logger->log(sese_tmp_event);                                            \
    }
#else
#define __SESE_LOG(point_to_logger, level, format, ...)              \
    {                                                                \
        auto sese_tmp_msg = sese::text::fmt(format, ##__VA_ARGS__);  \
        auto sese_tmp_time = sese::DateTime::now();                  \
        auto sese_tmp_event = std::make_shared<sese::record::Event>( \
                sese_tmp_time,                                       \
                level,                                               \
                sese::Thread::getCurrentThreadName(),                \
                sese::Thread::getCurrentThreadId(),                  \
                SESE_FILENAME,                                       \
                __LINE__,                                            \
                sese_tmp_msg                                         \
        );                                                           \
        point_to_logger->log(sese_tmp_event);                        \
    }
#endif

#define __SESE_RAW(point_to_logger, buffer, length) \
    point_to_logger->dump(buffer, length);          \
    SESE_MARCO_END

#define __SESE_EXCEPT(point_to_logger, e) \
    e.printStacktrace(point_to_logger)

#define __SESE_DEBUG(sese_tmp_logger, format, ...)                                 \
    __SESE_LOG(sese_tmp_logger, sese::record::Level::DEBUG, format, ##__VA_ARGS__) \
    SESE_MARCO_END

#define __SESE_INFO(sese_tmp_logger, format, ...)                                 \
    __SESE_LOG(sese_tmp_logger, sese::record::Level::INFO, format, ##__VA_ARGS__) \
    SESE_MARCO_END

#define __SESE_WARN(sese_tmp_logger, format, ...)                                 \
    __SESE_LOG(sese_tmp_logger, sese::record::Level::WARN, format, ##__VA_ARGS__) \
    SESE_MARCO_END

#define __SESE_ERROR(sese_tmp_logger, format, ...)                               \
    __SESE_LOG(sese_tmp_logger, sese::record::Level::ERR, format, ##__VA_ARGS__) \
    SESE_MARCO_END

#define SESE_RAW(buffer, length) \
    __SESE_RAW(sese::record::getLogger(), buffer, length)

#define SESE_EXCEPT(e) __SESE_EXCEPT(sese::record::getLogger(), e)

#define SESE_DEBUG(sese_tmp_format, ...)                                                              \
    __SESE_LOG(sese::record::getLogger(), sese::record::Level::DEBUG, sese_tmp_format, ##__VA_ARGS__) \
    SESE_MARCO_END

#define SESE_INFO(sese_tmp_format, ...)                                                              \
    __SESE_LOG(sese::record::getLogger(), sese::record::Level::INFO, sese_tmp_format, ##__VA_ARGS__) \
    SESE_MARCO_END

#define SESE_WARN(sese_tmp_format, ...)                                                              \
    __SESE_LOG(sese::record::getLogger(), sese::record::Level::WARN, sese_tmp_format, ##__VA_ARGS__) \
    SESE_MARCO_END

#define SESE_ERROR(sese_tmp_format, ...)                                                            \
    __SESE_LOG(sese::record::getLogger(), sese::record::Level::ERR, sese_tmp_format, ##__VA_ARGS__) \
    SESE_MARCO_END
