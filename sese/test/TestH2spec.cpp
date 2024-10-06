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

#include <gtest/gtest.h>

#include <sese/security/SSLContextBuilder.h>
#include <sese/service/http/HttpServer.h>
#include <sese/system/ProcessBuilder.h>
#include <sese/Log.h>

#include <filesystem>

TEST(TestH2spec, TestH2spec) {
#ifdef _WIN32
    std::string path_to_h2spec = PROJECT_PATH "/h2spec.exe";
#else
    std::string path_to_h2spec = PROJECT_PATH "/h2spec";
#endif
    if (!std::filesystem::exists(path_to_h2spec)) {
        GTEST_SKIP() << "Cannot find h2spec file";
    }

    auto addr = sese::net::IPv4Address::localhost(sese::net::createRandomPort());
    auto ssl = sese::security::SSLContextBuilder::UniqueSSL4Server();
    ASSERT_TRUE(ssl->importCertFile(PROJECT_PATH "/sese/test/Data/test-ca.crt"));
    ASSERT_TRUE(ssl->importPrivateKeyFile(PROJECT_PATH "/sese/test/Data/test-key.pem"));

    auto server = sese::service::http::HttpServer();
    server.regService(addr, std::move(ssl->copy()));
    ASSERT_TRUE(server.startup());

    sese::sleep(2s);

    auto process = sese::system::ProcessBuilder(path_to_h2spec)
                       .args("-j " PROJECT_PATH "/h2spec_report.xml")
                       .args("-p" + std::to_string(addr->getPort()))
                       .args("-t")
                       .args("-k")
                       .create();

    SESE_INFO("h2spec exit with {}", process->wait());
    server.shutdown();
}