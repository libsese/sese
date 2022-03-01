#pragma once
#include "record/ConsoleAppender.h"
#include "record/Logger.h"
#include "record/SimpleFormatter.h"
#include "util/Singleton.h"

namespace sese {

    extern API Logger *getLogger() noexcept;

    extern API bool isSpace(char ch) noexcept;

    extern API const char *getLevelString(Level level) noexcept;

    extern API pid_t getThreadId() noexcept;

    extern API const char *getThreadName() noexcept;
}// namespace sese

#ifdef _WIN32
#define sprintf sprintf_s
#endif

#define ROOT_LOG(LEVEL, ...)                                                                                                                        \
    {                                                                                                                                               \
        char buf[LOGGER_OUTPUT_BUFFER];                                                                                                             \
        sprintf(buf, __VA_ARGS__);                                                                                                                  \
        time_t tm;                                                                                                                                  \
        time(&tm);                                                                                                                                  \
        sese::Event::Ptr event = std::make_shared<sese::Event>(tm, LEVEL, sese::getThreadName(), sese::getThreadId(), __FILE__, __LINE__, "Hello"); \
        sese::Logger *logger = sese::getLogger();                                                                                                   \
        logger->log(event);                                                                                                                         \
    }

#define ROOT_DEBUG(...) \
    ROOT_LOG(sese::Level::DEBUG, __VA_ARGS__)

#define ROOT_INFO(...) \
    ROOT_LOG(sese::Level::INFO, __VA_ARGS__)

#define ROOT_WARN(...) \
    ROOT_LOG(sese::Level::WARN, __VA_ARGS__)

#define ROOT_ERROR(...) \
    ROOT_LOG(sese::Level::ERR, __VA_ARGS__)
