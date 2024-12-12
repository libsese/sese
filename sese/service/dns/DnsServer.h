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

/// @file DnsServer.h
/// @brief DNS Server
/// @author kaoru
/// @date October 30, 2024

#pragma once

#include "Config.h"

#include <sese/service/Service.h>

namespace sese::service::dns {

/// @brief DNS Server
class DnsServer final {
    Service::Ptr service;

public:
    /// @brief Constructor
    DnsServer();

    /// @brief Bind address
    /// @param address Address
    /// @return Whether binding is successful
    bool bind(const net::IPAddress::Ptr &address);

    /// @brief Set callback
    /// @param callback Callback
    void setCallback(const Callback &callback);

    /// @brief Add upstream server
    /// @param address Address
    void addUpstreamNameServer(const net::IPAddress::Ptr &address);

    /// @brief Add upstream server
    /// @param ip Address
    /// @param port Port
    /// @return Whether adding is successful
    bool addUpstreamNameServer(const std::string &ip, uint16_t port = 53);

    /// @brief Add record
    /// @param name Domain name
    /// @param address Address
    void addRecord(const std::string &name, const net::IPAddress::Ptr &address);

    /// @brief Startup
    /// @return Whether startup is successful
    bool startup();

    /// @brief Shutdown
    /// @return Whether shutdown is successful
    bool shutdown();

    /// @brief Get error code
    /// @return Error code
    int getLastError();

    /// @brief Get error message
    /// @return Error message
    std::string getLastErrorMessage();
};

} // namespace sese::service::dns

