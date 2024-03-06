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


#include "sese/service/HttpServiceFactory.h"
#include "sese/security/SSLContextBuilder.h"
#include "sese/net/Socket.h"
#include "sese/record/Marco.h"

#include <gtest/gtest.h>


TEST(TestHttpService, WithSSL) {
    auto ip_address = sese::net::IPv4Address::localhost(443);
    auto ssl_context = sese::security::SSLContextBuilder::SSL4Server();
    ASSERT_TRUE(ssl_context->importCertFile(PROJECT_PATH "/test/Data/test-ca.crt"));
    ASSERT_TRUE(ssl_context->importPrivateKeyFile(PROJECT_PATH "/test/Data/test-key.pem"));
    ASSERT_TRUE(ssl_context->authPrivateKey());
    auto handle = [](sese::service::HttpSession *session) {
        auto &&url = session->req().getUrl();
        SESE_DEBUG("url: %s", url.c_str());
        if (url == "/index.html") {
            session->resp().getBody().write("<h1>Hello, World</h1>", 21);
        } else {
            session->resp().setCode(404);
        }
    };

    auto service = sese::service::HttpServiceFactory::createHttpService(
            ip_address,
            ssl_context,
            60,
            4,
            MTU_VALUE,
            handle
    );

    ASSERT_TRUE(service->startup()) << service->getLastError();
    getchar();
    service->shutdown();
}