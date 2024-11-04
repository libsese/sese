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
* @file Config.h
* @brief Linux 平台配置文件
* @author kaoru
* @date 2022年3月28日
*/
#pragma once

#if defined(__x86_64__)
#define SESE_ARCH_X64
#elif defined(__aarch64__) || defined(__arm__)
#define SESE_ARCH_ARM64
#else
#error only support x86_64 & arm64
#endif

/// C++ 版本标识符
#define SESE_CXX_STANDARD __cplusplus
/// 可导出类型标识符
#define API
/// pid_t 格式化占位符
#define PRIdTid "d"
/// 忽略大小写比较
#define strcmpi strcasecmp

#include <cstring>
#include <memory>
#include <thread>

#define SESE_FILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

namespace sese {
/// Native Socket 类型
using socket_t = int32_t;
using tid_t = uint32_t;
using pid_t = ::pid_t;
} // namespace sese

#ifndef __GLIBC__
#define __MUSL__
#endif