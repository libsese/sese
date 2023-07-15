/**
 * @file Test.h
 * @author kaoru
 * @date 2022年3月28日
 * @brief 调试杂项函数
 */
#pragma once

#include "sese/util/Initializer.h"

#include <atomic>
#include <functional>

// #undef assert

#ifdef _WIN32
#define WILL_SKIP 1
#endif

#ifdef __linux__
#define WILL_SKIP 2
#endif

#ifdef __APPLE__
#define WILL_SKIP 2
#endif

namespace sese {

    namespace record {
        class LogHelper;
    }

    class API Test {
    public:
        /**
          * @brief 回溯程序堆栈保存到字符串
          * @param size 保存个个数
          * @param prefix 字符串前缀
          * @param skip 跳过个数
          * @return 调试信息
          */
        static std::string backtrace2String(int size, const std::string &prefix, int skip = WILL_SKIP);

        /**
         * @brief 替代断言宏方案
         * @param log 日志工具
         * @param expr 条件表达式
         * @param exitCode 命中时的退出退出码，为零则不退出
         * @deprecated 已弃用
         */
        // static void assert(record::LogHelper log, bool expr, int32_t exitCode = 0);

        /**
         * @brief 可自定义的断言方案
         * @ref assertion maroc
         * @param log 日志工具
         * @param firstLine 首行消息格式
         * @param expr 条件表达式
         * @param exitCode 命中时的退出退出码，为零则不退出
         * @deprecated 已弃用
         */
        // static void assert(record::LogHelper log, const char *firstLine, bool expr, int32_t exitCode = 0);
    };


    // class TestInitiateTask final : public InitiateTask {
    // public:
    //     TestInitiateTask() : InitiateTask(__FUNCTION__) {}
    //     int32_t init() noexcept override;
    //     int32_t destroy() noexcept override;
    // };
}// namespace sese

/**
 * @brief 断言宏
 * @anchor assertion maroc
 * @param LogHelper 日志器
 * @param Expr 条件表达式
 * @param ExitCode 命中时的退出退出码，为零则不退出
 * @deprecated 已弃用
 */
// #define assert(LogHelper, Expr, ExitCode) sese::Test::assert(LogHelper, "Assertion failed on \"" #Expr "\"\n%s", Expr, ExitCode);

#undef WILL_SKIP