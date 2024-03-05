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
    auto ip_address = sese::net::IPv4Address::localhost(8080);
    auto ssl_context = sese::security::SSLContextBuilder::SSL4Server();
    ASSERT_TRUE(ssl_context->importCertFile(PROJECT_PATH "/test/Data/test-ca.crt"));
    ASSERT_TRUE(ssl_context->importPrivateKeyFile(PROJECT_PATH "/test/Data/test-key.pem"));
    ASSERT_TRUE(ssl_context->authPrivateKey());
    auto handle = [](sese::service::HttpSession *session) {
        SESE_DEBUG("url: %s", session->req().getUrl().c_str());
        session->resp().getBody().write("Hello, World", 12);
    };

    auto service = sese::service::HttpServiceFactory::createHttpService(
            ip_address,
            ssl_context,
            60,
            MTU_VALUE,
            handle
    );

    ASSERT_TRUE(service->startup()) << service->getLastError();
    getchar();
    service->shutdown();
}