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

#include "sese/net/http/RequestableFactory.h"
#include "sese/net/http/RequestParser.h"
#include "sese/internal/net/http/HttpClientImpl.h"
#include "sese/internal/net/http/HttpSSLClientImpl.h"

using namespace sese::net::http;

std::unique_ptr<Requestable> RequestableFactory::createHttpRequest(const std::string &url, const std::string &proxy) {
    bool ssl;
    auto url_result = RequestParser::parse(url);
    IPAddress::Ptr address;
    // 不方便CI测试
    // GCOVR_EXCL_START
    if (!proxy.empty()) {
        auto proxy_result = RequestParser::parse(proxy);
        if (proxy_result.address == nullptr) {
            return nullptr;
        }
        url_result.request->setUrl(url);
        url_result.request->set("via:", proxy);
        url_result.request->set("proxy-connection", "keep-alive");
        address = std::move(proxy_result.address);
        ssl = strcmpDoNotCase("https", proxy_result.url.getProtocol().c_str());
    }
    // GCOVR_EXCL_STOP
    else {
        if (url_result.address == nullptr) {
            return nullptr;
        }
        address = std::move(url_result.address);
        ssl = strcmpDoNotCase("https", url_result.url.getProtocol().c_str());
    }

    url_result.request->set("user-agent", "sese-httpclient/1.0");
    url_result.request->set("connection", "keep-alive");

    if (ssl) {
        return std::make_unique<internal::net::http::HttpSSLClientImpl>(address, std::move(url_result.request));
    }
    return std::make_unique<internal::net::http::HttpClientImpl>(address, std::move(url_result.request));
}