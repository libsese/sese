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

#include <sese/net/http/HeaderBuilder.h>

using sese::net::http::Header;
using sese::net::http::HeaderBuilder;

HeaderBuilder::HeaderBuilder(Header &header) : header(header) {}

HeaderBuilder &&HeaderBuilder::set(const std::string &key, const std::string &value) && {
    header.set(key, value);
    return std::move(*this);
}
