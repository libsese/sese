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
 * @brief Darwin platform configuration file
 * @author kaoru
 * @date March 28, 2022
 */

#pragma once

#if defined(__x86_64__)
#define SESE_ARCH_X64
#elif defined(__aarch64__)
#define SESE_ARCH_ARM64
#else
#error only support x86_64 & arm64
#endif

/// C++ version identifier
#define SESE_CXX_STANDARD __cplusplus
/// Type identifiers can be exported
#define API
/// pid_t Formatting placeholders
#define PRIdTid "d"
/// Ignore case comparisons
#define strcmpi strcasecmp

#include <cstring>
#include <memory>
#include <thread>

#define SESE_FILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

namespace sese {
/// Native Socket type
using socket_t = int32_t;
using tid_t = uint32_t;
using pid_t = ::pid_t;
} // namespace sese
