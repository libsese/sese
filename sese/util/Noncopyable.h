/**
 * @file Noncopyable.h
 * @author kaoru
 * @date 2022年3月28日
 * @brief 不可复制类
 */
#pragma once

// GCOVR_EXCL_START

namespace sese {

/**
 * @brief 不可复制类
 */
class Noncopyable {
public:
    Noncopyable() = default;
    virtual ~Noncopyable() = default;

    /// 删除拷贝相关构造函数
    Noncopyable(const Noncopyable &) = delete;
    /// 删除拷贝相关构造函数
    Noncopyable &operator=(const Noncopyable &) = delete;
};
} // namespace sese

// GCOVR_EXCL_STOP