/**
 * @file Util.h
 * @brief 工具杂项函数
 * @author kaoru
 * @date 2022年3月28日
 */
#pragma once
#include "Config.h"
#include "Singleton.h"
#include "record/Logger.h"
#include "thread/Thread.h"
#include "Test.h"

namespace sese {

    /**
     * @brief 字符比较器
     */
    struct StrCmp {
        bool operator()(char const *lv, char const *rv);
    };

    /**
     * @brief 字符比较器（忽略大小写）
     */
    struct StrCmpI {
        bool operator()(char const *lv, char const *rv);
    };

    /**
     * 获取全局 Logger 指针
     * @return Logger 指针
     */
    extern API Logger *getLogger() noexcept;

    /**
     * 判断字符是否属于空白
     * @param ch 欲判断的字符
     * @return 真、假
     */
    extern API bool isSpace(char ch) noexcept;

    /**
     * 根据日志等级返回对应的字符串
     * @param level 日志等级
     * @return 字符串
     */
    extern API const char *getLevelString(Level level) noexcept;

    /**
     * 查找某字符在字符串中第一次出现的位置
     * @param str 在哪个字符串中
     * @param ch 欲查找的字符
     * @return 第一次出现的位置，没有则返回 -1
     */
    extern API int32_t findFirstAt(const char *str, char ch);

    /**
     * 使当前线程休眠一段时间，NATIVE API 实现
     * @param second 秒
     */
    extern API void sleep(uint32_t second);

    extern API std::string getErrorString(int32_t error = errno);
}// namespace sese

#ifdef _WIN32
#pragma warning(disable : 4996)
#endif

#ifdef __llvm__
#define FN __FILE_NAME__
#else
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

/// 输出 ERROR 级别的日志
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
        ROOT_ERROR(FILTER, "%s", sese::backtrace2String(5, "Backtrace ").c_str()); \
        assert(x);                                                                            \
    }

/**
 * @brief 更快速的边界检查 - [0, max)
 */
#define CheckRange(x, max) ((unsigned int) (x) < (max))

/**
 * @brief 更快速的边界检查 - [min, max]
 */
#define CheckRangeBetween(x, min, max) (((x) - (min)) | ((max) - (x)) >= 0)