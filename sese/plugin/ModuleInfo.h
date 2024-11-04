// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
} // namespace sese::plugin
