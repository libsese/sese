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

/// \file Util.h
/// \brief Utility Collection
/// \author kaoru
/// \version 0.1

#pragma once

#include <map>
#include <string>

namespace sese::db {

    /// \brief Perform simple tokenization on a string
    /// \param string Target string
    /// \return Tokenization result
    std::map<std::string, std::string> tokenize(const char *string) noexcept;
}