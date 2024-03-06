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

#include <sese/service/HttpServiceFactory.h>
#include <sese/internal/service/AsioHttpService.h>

std::unique_ptr<sese::service::Service> sese::service::HttpServiceFactory::createHttpService(
        const net::IPAddress::Ptr &addr,
        const security::SSLContext::Ptr &ssl,
        size_t keep_alive,
        size_t threads,
        size_t max_buffer_size,
        const std::function<void(HttpSession *)> &callback
) {
    return std::make_unique<internal::service::AsioHttpService>(addr, ssl, keep_alive, threads, max_buffer_size, callback);
}
