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
 * @file EncodingConverter.h
 * @author kaoru
 * @date April 15, 2022
 * @brief String encoding conversion
 */

#pragma once

#include "sese/Config.h"
#include "sese/util/NotInstantiable.h"
#include "sese/util/Initializer.h"

#ifdef _WIN32
#pragma warning(disable : 4996)
#pragma warning(disable : 4624)
#endif

namespace sese {

/**
 * @brief String encoding conversion
 */
class SESE_DEPRECATED EncodingConverter final : public NotInstantiable {
public:
    EncodingConverter() = delete;

    [[maybe_unused]] static std::string toString(const std::wstring &wstring) noexcept;
    [[maybe_unused]] static std::wstring toWstring(const std::string &string) noexcept;
};

/// Encoding converter initialization task
class EncodingConverterInitiateTask final : public InitiateTask {
public:
    EncodingConverterInitiateTask() : InitiateTask(__FUNCTION__) {}

    int32_t init() noexcept override;
    int32_t destroy() noexcept override;
};
} // namespace sese