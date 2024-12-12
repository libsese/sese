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

/// \file RequestParser.h
/// \author kaoru
/// \brief Request Parser
/// \date October 14, 2023

#pragma once

#include <sese/net/IPv6Address.h>
#include <sese/net/http/Request.h>
#include <sese/net/http/UrlHelper.h>

namespace sese::net::http {

/// Request Parser
class RequestParser {
public:
    /// Request Parse Result
    struct Result {
        Url url;
        IPAddress::Ptr address{};
        Request::Ptr request{};
    };

    static Result parse(const std::string &url);

    static IPv4Address::Ptr parseAddress(const std::string &host) noexcept;
};

} // namespace sese::net::http
