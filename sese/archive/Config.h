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

/// \file Config.h
/// \brief Archive 相关配置
/// \author kaoru
/// \date 2024年04月10日

#pragma once

namespace sese::archive {
    /// @brief Archive 配置类
    class Config {
    public:
        enum class EntryType {
            MT,
            REG,
            LNK,
            SOCK,
            CHR,
            BLK,
            DIR,
            IFO
        };
        static int toValue(EntryType type);
        static EntryType fromValue(int value);
    };
}