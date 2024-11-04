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

#include <sese/net/http/Request.h>

// GCOVR_EXCL_START

void sese::net::http::Request::swap(sese::net::http::Request &another) noexcept {
    std::swap(type, another.type);
    std::swap(uri, another.uri);
    std::swap(version, another.version);
    std::swap(cookies, another.cookies);
    std::swap(headers, another.headers);
    std::swap(query_args, another.query_args);
    body.swap(another.body);
}

// GCOVR_EXCL_STOP