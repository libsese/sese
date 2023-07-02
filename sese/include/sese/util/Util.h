/**
 * @file Util.h
 * @brief 工具杂项函数
 * @author kaoru
 * @date 2022年3月28日
 */

#pragma once

#include "sese/Config.h"

namespace sese {

    // 使用预声明减少头文件引用
    enum class Level;

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

    /**
     * 获取报错信息
     * @param error 错误代码
     * @return 错误描述
     */
    extern API std::string getErrorString(int32_t error = errno);

    /**
     * 获取 error 代码
     * @return error 代码
     */
    extern API uint64_t getErrorCode();

}// namespace sese

/**
 * 获取详细的信息(C 接口)
 * @return 版本信息字符串
 */
extern "C" const char *getSpecificVersion();

#ifdef _WIN32
#pragma warning(disable : 4996)
#endif

/**
 * @brief 更快速的边界检查 - [0, max)
 */
#define CheckRange(x, max) ((unsigned int) (x) < (max))

/**
 * @brief 更快速的边界检查 - [min, max]
 */
#define CheckRangeBetween(x, min, max) (((x) - (min)) | ((max) - (x)) >= 0)