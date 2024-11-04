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

#include <sese/net/http/UrlHelper.h>

using sese::net::http::Url;

Url::Url(const std::string &url) noexcept {
    // 协议
    auto protocol_end = url.find("://", 0);
    if (protocol_end != std::string::npos) {
        protocol = std::string_view(url.data(), protocol_end);
        protocol_end += 3;
    } else {
        protocol_end = 0;
    }

    // 域名
    bool found_host = false;
    auto host_end = url.find('/', protocol_end);
    if (host_end != std::string::npos) {
        found_host = true;
        host = std::string_view(url.data() + protocol_end, host_end - protocol_end);
    } else {
        host_end = protocol_end;
    }

    // 资源 & 查询字符串
    auto uri_end = url.find('?', host_end);
    if (uri_end == std::string::npos) {
        // 无查询字符串
        if (!found_host) {
            host = std::string_view(url.data() + host_end);
            this->url = "/";
        } else {
            this->url = std::string_view(url.data() + host_end);
        }
    } else {
        // 有查询字符串
        if (!found_host) {
            host = std::string_view(url.data() + host_end, uri_end - host_end);
            this->url = "/";
        } else {
            this->url = std::string_view(url.data() + host_end, uri_end - host_end);
        }
        query = std::string_view(url.data() + uri_end);
    }
}