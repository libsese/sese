/// \file HttpServiceFactory.h
/// \brief Http 服务工厂类
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

#include <sese/service/Service.h>
#include <sese/service/HttpSession.h>
#include <sese/security/SSLContext.h>
#include <sese/net/IPv6Address.h>
#include <sese/util/NotInstantiable.h>

#include <functional>

namespace sese::service {
class HttpServiceFactory : public NotInstantiable {
public:
    static std::unique_ptr<Service> createHttpService(
            const net::IPAddress::Ptr &addr,
            const security::SSLContext::Ptr &ssl,
            size_t keep_alive,
            size_t max_buffer_size,
            const std::function<void(HttpSession *)> &callback);
};
}