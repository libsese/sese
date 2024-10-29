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

#include "Config.h"

#include <sese/service/service.h>

namespace sese::service::dns {

class DnsServer final {
    Service::Ptr service;

public:
    DnsServer();

    bool bind(const net::IPAddress::Ptr &address);

    void setCallback(const Callback &callback);

    void addUpstreamNameServer(const net::IPAddress::Ptr &address);

    bool addUpstreamNameServer(const std::string &ip, uint16_t port = 53);

    void addRecord(const std::string &name, const net::IPAddress::Ptr &address);

    bool startup();

    bool shutdown();

    int getLastError();

    std::string getLastErrorMessage();
};

} // namespace sese::service::dns