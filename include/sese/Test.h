/**
 * @file Test.h
 * @author kaoru
 * @date 2022年3月28日
 * @brief 调试杂项函数
 */
#pragma once
#include "sese/Config.h"
#include <functional>

#ifdef _WIN32
#define WILL_SKIP 1
#endif

#ifdef __linux__
#define WILL_SKIP 2
#endif

#ifdef __APPLE__
#define WILL_SKIP 2
#endif

#undef assert

namespace sese {

    class LogHelper;
    class API Test {
    public:
        /**
          * @breif 回溯程序堆栈保存到字符串
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
         * @param callback 命中回调函数，默认执行 exit(-1)
         */
        static void assert(LogHelper log, bool expr, const std::function<void()>& callback = []() { exit(-1); });
    };
}// namespace sese