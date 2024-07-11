/**
* @file SecurityConfig.h
* @author kaoru
* @version 0.1
* @brief 安全模块配置
* @date 2023年9月13日
*/

#pragma once

#include "sese/util/Initializer.h"

namespace sese::security {
/// 安全模块初始化任务
class  SecurityInitTask final : public InitiateTask {
public:
    SecurityInitTask() : InitiateTask(__FUNCTION__) {}

    int32_t init() noexcept override;

    int32_t destroy() noexcept override;
};
} // namespace sese::security