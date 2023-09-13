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

}// namespace sese

// GCOVR_EXCL_START
/// https://stackoverflow.com/questions/61030383/how-to-convert-stdfilesystemfile-time-type-to-time-t
template<typename TP>
std::time_t to_time_t(TP tp) {
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now() + system_clock::now());
    return system_clock::to_time_t(sctp);
}
// GCOVR_EXCL_STOP

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