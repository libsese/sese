/**
 * @file ModuleInfo.h
 * @brief 模块基础信息
 * @author kaoru
 * @version 0.2
 */

#pragma once

#include "sese/plugin/ClassFactory.h"

namespace sese::plugin {
    /// 模块基础信息
    struct ModuleInfo {
        /// 模块名称
        const char *moduleName = nullptr;
        /// 模块版本
        const char *versionString = nullptr;
        /// 模块描述
        const char *description = nullptr;
    };
}// namespace sese::plugin
