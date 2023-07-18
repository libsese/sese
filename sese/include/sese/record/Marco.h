#pragma once

#include <sese/record/Logger.h>
#include <sese/thread/Thread.h>

#define __SESE_LOG(point_to_logger, level, format, ...)     \
    {                                                       \
        char buf[RECORD_OUTPUT_BUFFER]{0};                  \
        sprintf(buf, format, ##__VA_ARGS__);                \
        auto time = sese::DateTime::now();                  \
        auto event = std::make_shared<sese::record::Event>( \
                time,                                       \
                level,                                      \
                sese::Thread::getCurrentThreadName(),       \
                sese::Thread::getCurrentThreadId(),         \
                SESE_FILENAME,                              \
                __LINE__,                                   \
                buf                                         \
        );                                                  \
        point_to_logger->log(event);                        \
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
