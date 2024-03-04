/// \file HttpSession.h
/// \brief http 会话接口
/// \author kaoru
/// \date 2024年03月4日

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

#include <sese/net/http/Request.h>
#include <sese/net/http/Response.h>

namespace sese::service {
class HttpSession {
public:
    net::http::Request &req() { return request; };
    net::http::Response &resp() { return response; }

    virtual ~HttpSession() = default;

private:
    net::http::Request request{};
    net::http::Response response{};
};
} // namespace sese::service