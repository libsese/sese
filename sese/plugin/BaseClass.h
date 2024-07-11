/**
 * @file BaseClass.h
 * @brief 模块基类
 * @author kaoru
 * @version 0.2
 */

#pragma once

#include <memory>

// GCOVR_EXCL_START

namespace sese::plugin {
/// 模块基类
class BaseClass : public std::enable_shared_from_this<BaseClass> {
public:
    using Ptr = std::shared_ptr<BaseClass>;

    virtual ~BaseClass() = default;
};
} // namespace sese::plugin

// GCOVR_EXCL_STOP