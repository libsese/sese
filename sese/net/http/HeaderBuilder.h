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

/// \file HeaderBuilder.h
/// \brief Header Key-Value Collection Builder
/// \author kaoru
/// \date October 31, 2023

#pragma once

#include <sese/net/http/Header.h>

namespace sese::net::http {

/// Header Key-Value Collection Builder
class HeaderBuilder {
    Header &header;

public:
    explicit HeaderBuilder(Header &header);
    HeaderBuilder &operator=(HeaderBuilder &&other) = delete;
    HeaderBuilder(const HeaderBuilder &other) = delete;
    const HeaderBuilder &operator=(const HeaderBuilder &) = delete;

    HeaderBuilder &&set(const std::string &key, const std::string &value) &&;
};

} // namespace sese::net::http