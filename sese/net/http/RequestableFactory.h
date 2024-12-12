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

/// \file RequestableFactory.h
/// \brief Requestable Factory
/// \author kaoru
/// \date February 23, 2024

#pragma once

#include <sese/net/http/Requestable.h>
#include <sese/util/NotInstantiable.h>

namespace sese::net::http {
/// Requestable Factory
class RequestableFactory final : public NotInstantiable {
public:
    RequestableFactory() = delete;

    /// Create a standard requestable type
    /// \param url Request URL, supports https/http protocols
    /// \param proxy Proxy URL, supports https/http protocols, if empty, no proxy is used
    /// \return Requestable type, returns nullptr if creation fails
    static std::unique_ptr<Requestable> createHttpRequest(const std::string &url, const std::string &proxy = "");
};
} // namespace sese::net::http
