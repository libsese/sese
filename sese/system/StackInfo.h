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

/// \file StackInfo.h
/// \brief Stack Information Class
/// \author kaoru
/// \date September 9, 2023

#pragma once

#include <sese/Config.h>

#include <vector>

#if defined(_MSC_VER)
#pragma warning(disable : 4251)
#endif

namespace sese::system {

/// Sub-Stack Information
struct SubStackInfo {
    uint64_t address{};
    std::string func{};
};

/// Stack Information
class StackInfo final {
public:
    /// Create a container to store the current stack information
    /// \param limit Stack depth limit
    /// \param skip Skip depth size
    explicit StackInfo(uint16_t limit, uint16_t skip) noexcept;

    SESE_STD_WRAPPER(begin, stacks)
    SESE_STD_WRAPPER(end, stacks)
    SESE_STD_WRAPPER(rbegin, stacks)
    SESE_STD_WRAPPER(rend, stacks)
    SESE_STD_WRAPPER(empty, stacks)
    SESE_STD_WRAPPER(size, stacks)

    static uint16_t offset;

protected:
    /// \brief Decode symbol name
    /// \details This function is used to remove template parameter information on Windows,
    /// and to decode symbol names on other systems.
    /// \param str Symbol name
    /// \return Decoded symbol name
    static std::string decodeSymbolName(const std::string &str) noexcept;

    std::vector<SubStackInfo> stacks;
};

} // namespace sese::system

