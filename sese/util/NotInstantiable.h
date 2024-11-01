/**
 * @file NotInstantiable.h
 * @brief 不可实例化类
 * @author kaoru
 * @date 2022年3月28日
 */
#pragma once

namespace sese {

/**
 * @brief 不可实例化类
 */
class NotInstantiable {
public:
    NotInstantiable() = delete;
    virtual ~NotInstantiable() = default;
    NotInstantiable(const NotInstantiable &) = delete;
    NotInstantiable &operator=(const NotInstantiable &) = delete;
};
} // namespace sese