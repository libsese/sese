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

#include <sese/util/NotInstantiable.h>

#include <sese/net/http/Request.h>
#include <sese/net/http/Response.h>

namespace sese::net::http {

    class HttpConverter final : public NotInstantiable {
    public:
        HttpConverter() = delete;

        static bool convertFromHttp2(Request *request);

        static void convert2Http2(Response *response);
    };

}
