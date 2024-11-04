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

#include <sese/net/http/RequestParser.h>
#include <sese/net/AddressPool.h>
#include <sese/text/StringBuilder.h>
#include <sese/util/Util.h>

sese::net::http::RequestParser::Result sese::net::http::RequestParser::parse(const std::string &url) {
    Result result;
    result.url = Url(url);

    uint16_t port = 80;
    if (sese::StrCmpI()(result.url.getProtocol().c_str(), "Https") == 0) {
        port = 443;
    }

    IPv4Address::Ptr addr;
    if (result.url.getHost().find(':') == std::string::npos) {
        addr = parseAddress(result.url.getHost());
    } else {
        auto split_result = text::StringBuilder::split(result.url.getHost(), ":");
        if (split_result.size() == 2) {
            char *end_ptr;
            port = static_cast<uint16_t>(std::strtol(split_result[1].c_str(), &end_ptr, 10));
            if (*end_ptr != 0) {
                return result;
            }
            addr = parseAddress(split_result[0]);
        } else {
            return result;
        }
    }

    if (!addr) {
        return result;
    }

    addr->setPort(port);
    result.address = addr;

    result.request = std::make_unique<Request>();
    result.request->set("host", result.url.getHost());
    result.request->setUrl(result.url.getUrl() + result.url.getQuery());

    return result;
}

sese::net::IPv4Address::Ptr sese::net::http::RequestParser::parseAddress(const std::string &host) noexcept {
    auto result = text::StringBuilder::split(host, ".");
    if (result.size() == 4) {
        for (decltype(auto) item: result) {
            char *end_ptr;
            auto bit = std::strtol(item.c_str(), &end_ptr, 10);
            if (*end_ptr != 0) {
                goto lookup;
            }
            if (bit < 0 || bit > 255) {
                goto lookup;
            }
        }
        return IPv4Address::create(host.c_str(), 0);
    }

lookup:
    return IPv4AddressPool::lookup(host);
}