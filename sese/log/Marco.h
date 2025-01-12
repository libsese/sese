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
 * @file Marco.h
 * @author kaoru
 * @version 0.1
 * @brief Log macros
 * @date September 13, 2023
 */

#pragma once

#include <sese/log/Logger.h>

#define SESE_RAW(buffer, length) \
    sese::log::getLogger()->dump(buffer, length)

#define SESE_EXCEPT(e) e.printStacktrace(sese::log::getLogger())

#define SESE_DEBUG(sese_tmp_format, ...) \
    sese::log::Logger::debug(sese_tmp_format, ##__VA_ARGS__)

#define SESE_INFO(sese_tmp_format, ...) \
    sese::log::Logger::info(sese_tmp_format, ##__VA_ARGS__)

#define SESE_WARN(sese_tmp_format, ...) \
    sese::log::Logger::warn(sese_tmp_format, ##__VA_ARGS__)

#define SESE_ERROR(sese_tmp_format, ...) \
    sese::log::Logger::error(sese_tmp_format, ##__VA_ARGS__)
