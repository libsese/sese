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
 * \file UuidBuilder.h
 * \author kaoru
 * \date December 24, 2022
 * \version 0.1
 * \brief UUID Generation Related
 */

#pragma once

#ifdef WIN32
#pragma warning(disable : 4251)
#endif

#include "sese/Config.h"
#include "sese/util/TimestampHandler.h"
#include "sese/util/Uuid.h"

namespace sese {
/// Uuid Builder
class UuidBuilder {
public:
    /// Constructor
    explicit UuidBuilder() noexcept;

    /// Generate UUID
    /// \param dest UUID to store the result
    /// \return Whether generation is successful
    bool generate(sese::Uuid &dest) noexcept;

protected:
    sese::TimestampHandler timestampHandler;
};
} // namespace sese

