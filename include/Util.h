#pragma once
#include "Config.h"
#include "Singleton.h"
#include "Test.h"
#include "record/ConsoleAppender.h"
#include "record/Logger.h"
#include "record/SimpleFormatter.h"
#include "thread/Thread.h"

namespace sese {

    struct StrCmp {
        bool operator()(char const *lv, char const *rv);
    };

    struct StrCmpI {
        bool operator()(char const *lv, char const *rv);
    };

    extern API Logger *getLogger() noexcept;

    extern API bool isSpace(char ch) noexcept;

    extern API const char *getLevelString(Level level) noexcept;

    extern API int32_t findFirstAt(const char *str, char ch);

}// namespace sese

#ifdef _WIN32
#pragma warning(disable : 4996)
#endif

#ifdef __llvm__
#define FN __FILE_NAME__
#elif __GNUC__
#define FN __FILE__
#elif _MSC_VER
#define FN __FILE__
#endif

#define ROOT_LOG(LEVEL, FILTER, ...)                                                                                                                                                                       \
    {                                                                                                                                                                                                      \
        char buf[RECORD_OUTPUT_BUFFER];                                                                                                                                                                    \
        sprintf(buf, __VA_ARGS__);                                                                                                                                                                         \
        sese::Event::Ptr event = std::make_shared<sese::Event>(sese::DateTime::now(), LEVEL, sese::Thread::getCurrentThreadName().c_str(), sese::Thread::getCurrentThreadId(), FN, __LINE__, buf, FILTER); \
        sese::Logger *logger = sese::getLogger();                                                                                                                                                          \
        logger->log(event);                                                                                                                                                                                \
    }

#define ROOT_DEBUG(FILTER, ...) \
    ROOT_LOG(sese::Level::DEBUG, FILTER, __VA_ARGS__)

#define ROOT_INFO(FILTER, ...) \
    ROOT_LOG(sese::Level::INFO, FILTER, __VA_ARGS__)

#define ROOT_WARN(FILTER, ...) \
    ROOT_LOG(sese::Level::WARN, FILTER, __VA_ARGS__)

#define ROOT_ERROR(FILTER, ...) \
    ROOT_LOG(sese::Level::ERR, FILTER, __VA_ARGS__)

#if defined __GNUC__ || __llvm__
#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define LIKELY(x) (x)
#define UNLIKELY(x) (x)
#endif

#define ASSERT(FILTER, x)                                                                     \
    if (UNLIKELY(!(x))) {                                                                     \
        ROOT_ERROR(FILTER, "%s", sese::backtrace2String(5, WILL_SKIP, "Backtrace ").c_str()); \
        assert(x);                                                                            \
    }

// [0, max)
#define CheckRange(x, max) \
    ((unsigned int) (x) < (max))

// [min, max]
#define CheckRangeBetween(x, min, max) \
    (((x) - (min)) | ((max) - (x)) >= 0)