#pragma once

#include <sese/record/Logger.h>
#include <sese/thread/Thread.h>

#ifdef WIN32
#define sprintf sprintf_s
#endif

#define __SESE_LOG(point_to_logger, level, format, ...)       \
    {                                                         \
        char __buf[RECORD_OUTPUT_BUFFER]{0};                  \
        sprintf(__buf, format, ##__VA_ARGS__);                \
        auto __time = sese::DateTime::now();                  \
        auto __event = std::make_shared<sese::record::Event>( \
                __time,                                       \
                level,                                        \
                sese::Thread::getCurrentThreadName(),         \
                sese::Thread::getCurrentThreadId(),           \
                SESE_FILENAME,                                \
                __LINE__,                                     \
                __buf                                         \
        );                                                    \
        point_to_logger->log(__event);                        \
    }

#define __SESE_DEBUG(logger, format, ...)                                 \
    __SESE_LOG(logger, sese::record::Level::DEBUG, format, ##__VA_ARGS__) \
    SESE_MARCO_END

#define __SESE_INFO(logger, format, ...)                                 \
    __SESE_LOG(logger, sese::record::Level::INFO, format, ##__VA_ARGS__) \
    SESE_MARCO_END

#define __SESE_WARN(logger, format, ...)                                 \
    __SESE_LOG(logger, sese::record::Level::WARN, format, ##__VA_ARGS__) \
    SESE_MARCO_END

#define __SESE_ERROR(logger, format, ...)                               \
    __SESE_LOG(logger, sese::record::Level::ERR, format, ##__VA_ARGS__) \
    SESE_MARCO_END

#define SESE_DEBUG(format, ...)                                                              \
    __SESE_LOG(sese::record::getLogger(), sese::record::Level::DEBUG, format, ##__VA_ARGS__) \
    SESE_MARCO_END

#define SESE_INFO(format, ...)                                                              \
    __SESE_LOG(sese::record::getLogger(), sese::record::Level::INFO, format, ##__VA_ARGS__) \
    SESE_MARCO_END

#define SESE_WARN(format, ...)                                                              \
    __SESE_LOG(sese::record::getLogger(), sese::record::Level::WARN, format, ##__VA_ARGS__) \
    SESE_MARCO_END

#define SESE_ERROR(format, ...)                                                            \
    __SESE_LOG(sese::record::getLogger(), sese::record::Level::ERR, format, ##__VA_ARGS__) \
    SESE_MARCO_END
