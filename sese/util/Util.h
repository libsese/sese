/**
 * @file Util.h
 * @brief 工具杂项函数
 * @author kaoru
 * @date 2022年3月28日
 */

#pragma once

#include "sese/Config.h"
#include "sese/io/InputStream.h"
#include "sese/io/OutputStream.h"

#include <chrono>
#include <thread>

using namespace std::chrono_literals;

namespace sese {

// 使用预声明减少头文件引用
enum class Level;

/// 无视错误的整形转换函数
/// @param string 表示整形的字符串
/// @param radix 进制
/// @return 整形
int64_t toInteger(const std::string &string, int radix = 10);

/**
 * @brief 字符比较器
 */
struct StrCmp {
    int operator()(char const *lv, char const *rv);
};

/**
 * @brief 字符比较器（忽略大小写）
 */
struct StrCmpI {
    int operator()(char const *lv, char const *rv);
};

/**
 * 比较两个字符串是否相同
 * \param lv 字符串1
 * \param rv 字符串2
 * \return 比较结果
 */
API bool strcmp(char const *lv, char const *rv) noexcept;

/**
 * 比较两个字符串是否相同（忽略大小写）
 * \param lv 字符串1
 * \param rv 字符串2
 * \return 比较结果
 */
API bool strcmpDoNotCase(char const *lv, char const *rv) noexcept;

/**
 * 判断字符是否属于空白
 * @param ch 欲判断的字符
 * @return 是否属于空白
 */
API bool isSpace(char ch) noexcept;

/**
 * 根据日志等级返回对应的字符串
 * @param level 日志等级
 * @return 字符串
 */
API const char *getLevelString(Level level) noexcept;

/**
 * 查找某字符在字符串中第一次出现的位置
 * @param str 在哪个字符串中
 * @param ch 欲查找的字符
 * @return 第一次出现的位置，没有则返回 -1
 */
API int32_t findFirstAt(const char *str, char ch);

/**
 * 使当前线程休眠一段时间，NATIVE API 实现
 * @param second 秒
 */
API void sleep(uint32_t second);

/**
 * 使当前线程休眠一段时间，std 实现
 */
template<class REP, class PERIOD>
API void sleep(const std::chrono::duration<REP, PERIOD> &duration) {
    std::this_thread::sleep_for(duration);
}

/**
 * 获取报错信息
 * @param error 错误代码
 * @return 错误描述
 */
API std::string getErrorString(int64_t error = errno);

/**
 * 获取 error 代码
 * @return error 代码
 */
API int64_t getErrorCode();

/// 移动流数据
/// \param out 输出流
/// \param in 输入流
/// \param size 移动大小
/// \return 实际移动大小
API size_t streamMove(sese::io::OutputStream *out, sese::io::InputStream *in, size_t size) noexcept;

/// 获取数字转字符串后所需的字节长度
/// @note 只能转换整数
/// @tparam T 入参整数类型
/// @param num 待转换的数字
/// @param radix 进制
/// @return 数字字符串所需长度
template<class T>
size_t number2StringLength(T num, size_t radix = 10) {
    static_assert(!std::is_same_v<T, double>, "Must be an integer");
    static_assert(!std::is_same_v<T, float>, "Must be an integer");
    size_t length = 0;
    if (num == 0) return 1;
    if (num < 0) {
        length += 1;
        num *= -1;
    }
    do {
        num /= static_cast<T>(radix);
        length += 1;
    } while (num > 0);
    return length;
}

// GCOVR_EXCL_START
/// 时间类型转换
/// @see https://stackoverflow.com/questions/61030383/how-to-convert-stdfilesystemfile-time-type-to-time-t
/// @tparam TP 转换对象类型
/// @param tp 转换对象
/// @return std::time_t 类型的时间
template<typename TP>
std::time_t to_time_t(TP tp) {
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now() + system_clock::now());
    return system_clock::to_time_t(sctp);
}
// GCOVR_EXCL_STOP

} // namespace sese

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