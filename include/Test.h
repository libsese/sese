/**
 * @file Test.h
 * @author kaoru
 * @date 2022年3月28日
 * @brief 调试杂项函数
 */
#pragma once
#include "Config.h"
#include <string>

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
    /**
     * 回溯程序堆栈保存到字符串
     * @param size 保存个个数
     * @param skip 跳过个数
     * @param prefix 字符串前缀
     * @return 调试信息
     */
    extern API std::string backtrace2String(int size, int skip, const std::string &prefix);
}