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
* @file Response.h
* @author kaoru
* @version 0.1
* @brief HTTP 响应类
* @date 2023年9月13日
*/

#pragma once

#include <sese/net/http/ResponseHeader.h>
#include "sese/io/ByteBuilder.h"

namespace sese::net::http {

/// HTTP 响应类
class Response final : public ResponseHeader {
public:
    using Ptr = std::unique_ptr<Response>;

    io::ByteBuilder &getBody() { return body; }

    void swap(Response &another) noexcept;

private:
    io::ByteBuilder body{8192};
};

} // namespace sese::net::http