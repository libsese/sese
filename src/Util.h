#pragma once
#include "record/ConsoleAppender.h"
#include "record/Logger.h"
#include "record/SimpleFormatter.h"
#include "thread/Thread.h"
#include "util/Singleton.h"
#include "util/Test.h"
#include <cassert>

namespace sese {

    extern API Logger *getLogger() noexcept;

    extern API bool isSpace(char ch) noexcept;

    extern API const char *getLevelString(Level level) noexcept;

}// namespace sese

#ifdef _WIN32
#define sprintf sprintf_s
#endif

#ifdef __GNUC__
#define FN __FILE__
#elif __llvm__
#define FN __FILE_NAME__
#elif _MSC_VER
#define FN __FILE__
#endif

#define ROOT_LOG(LEVEL, ...)                                                                                                                                                                               \
    {                                                                                                                                                                                                      \
        char buf[LOGGER_OUTPUT_BUFFER];                                                                                                                                                                    \
        sprintf(buf, __VA_ARGS__);                                                                                                                                                                         \
        sese::Event::Ptr event = std::make_shared<sese::Event>(sese::DateTime::now(), LEVEL, sese::ThreadInfo::getCurrentThreadName().c_str(), sese::ThreadInfo::getCurrentThreadId(), FN, __LINE__, buf); \
        sese::Logger *logger = sese::getLogger();                                                                                                                                                          \
        logger->log(event);                                                                                                                                                                                \
    }

#define ROOT_DEBUG(...) \
    ROOT_LOG(sese::Level::DEBUG, __VA_ARGS__)

#define ROOT_INFO(...) \
    ROOT_LOG(sese::Level::INFO, __VA_ARGS__)

#define ROOT_WARN(...) \
    ROOT_LOG(sese::Level::WARN, __VA_ARGS__)

#define ROOT_ERROR(...) \
    ROOT_LOG(sese::Level::ERR, __VA_ARGS__)

#if defined __GNUC__ || __llvm__
#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define LIKELY(x) (x)
#define UNLIKELY(x) (x)
#endif

#define ASSERT(x)                                                                     \
    if (UNLIKELY(!(x))) {                                                             \
        ROOT_ERROR("%s", sese::backtrace2String(5, WILL_SKIP, "Backtrace ").c_str()); \
        assert(x);                                                                    \
    }

#define ASSERTEX(x, ...)                                                              \
    if (UNLIKELY(!(x))) {                                                             \
        ROOT_ERROR("%s", sese::backtrace2String(5, WILL_SKIP, "Backtrace ").c_str()); \
        ROOT_ERROR(__VA_ARGS__)                                                       \
        assert(x);                                                                    \
    }
