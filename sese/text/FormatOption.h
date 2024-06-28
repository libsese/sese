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

#pragma once

#include <sese/Config.h>

namespace sese::text {

enum class Align {
    LEFT,
    CENTER,
    RIGHT
};

struct FormatOption {
    Align align = Align::LEFT;
    uint16_t wide = 0;
    uint16_t float_accuracy = 0;
    char ext_type = 0;

    bool parse(const std::string &value);
};

} // namespace sese::text