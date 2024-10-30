/**
 * @file Environment.h
 * @brief 环境信息类
 * @author kaoru
 * @date 2022年3月28日
 */

#pragma once

#include "sese/util/NotInstantiable.h"

#include <string>

#ifdef _WIN32
#pragma warning(disable : 4624)
#endif

namespace sese::system {

/**
 * @brief 环境信息类
 */
class Environment final : public NotInstantiable {
public:
    Environment() = delete;

    /**
     * @return 返回仓库哈希码前 6 位
     */
    static const char *getRepoHash() noexcept;

    /**
     * @return 返回仓库所在分支
     */
    static const char *getRepoBranch() noexcept;

    /**
     * @return 返回主要版本号
     */
    static const char *getMajorVersion() noexcept;

    /**
     * @return 返回次要版本号
     */
    static const char *getMinorVersion() noexcept;

    /**
     * @return 返回补丁版本号
     */
    static const char *getPatchVersion() noexcept;

    /**
     * @return 返回构建日期字符串
     */
    static const char *getBuildDate() noexcept;

    /**
     * @return 返回构建时间字符串
     */
    static const char *getBuildTime() noexcept;

    /**
     * @return 返回构建时间的时间戳
     */
    static const char *getBuildDateTime() noexcept;

    /**
     * @return 返回构建的操作系统类型
     */
    static const char *getOperateSystemType() noexcept;

    /**
     * @return 返回 CPU 的大小端模式
     */
    static bool isLittleEndian() noexcept;

    /**
     * 设置当前上下文环境变量
     * @param key 键
     * @param value 值
     * @return 是否设置成功
     */
    static bool setEnv(const std::string &key, const std::string &value) noexcept;

    /**
     * 取消设置当前上下文环境变量
     * @param key 键
     * @return 是否设置成功
     */
    static bool unsetEnv(const std::string &key) noexcept;

    /**
     * 获取当前上下文环境变量
     * @param key 键
     * @return 获取到的值，需要调用 std::string::empty() 判空
     */
    static std::string getEnv(const std::string &key) noexcept;
};
} // namespace sese::system