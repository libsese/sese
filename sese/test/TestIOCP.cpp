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

#include <sese/service/iocp/IOCPServer.h>
#include <sese/security/SSLContextBuilder.h>
#include <sese/record/Marco.h>
#include <sese/util/Util.h>

class MyIOCPServer : public sese::iocp::IOCPServer {
public:
    MyIOCPServer() {
        setDeleteContextCallback(myDeleter);
    }

    void onAcceptCompleted(sese::iocp::Context *ctx) override {
        SESE_INFO("onAcceptCompleted {}", ctx->getFd());
        postRead(ctx);
    }

    void onPreRead(sese::iocp::Context *ctx) override {
        SESE_INFO("onRreRead {}", ctx->getFd());
    }

    void onReadCompleted(sese::iocp::Context *ctx) override {
        SESE_INFO("onReadCompleted {}", ctx->getFd());
        sese::streamMove(ctx, ctx, IOCP_WSABUF_SIZE);
        postWrite(ctx);
    }

    void onWriteCompleted(sese::iocp::Context *ctx) override {
        SESE_INFO("onWriteCompleted {}", ctx->getFd());
    }

    void onConnected(sese::iocp::Context *ctx) override {
        SESE_INFO("onConnected {}", ctx->getFd());
    }

    static void myDeleter(sese::iocp::Context *ctx) {
        SESE_INFO("onDeleteCallback {}", ctx->getFd());
    }
};

TEST(TestIOCP, Server_0) {
    auto address = sese::net::IPv4Address::localhost(sese::net::createRandomPort());

    MyIOCPServer server;
    server.setAddress(address);
    server.setThreads(2);
    server.setServProtos("\x8http/1.1");
    ASSERT_TRUE(server.init());
    SESE_INFO("server listening on {}", address->getPort());

    sese::net::Socket socket(sese::net::Socket::Family::IPv4, sese::net::Socket::Type::TCP);
    ASSERT_EQ(socket.connect(address), 0);
    constexpr auto s = "Hello World";
    socket.write(s, strlen(s));

    char buffer[32]{};
    socket.read(buffer, sizeof(buffer));
    SESE_INFO("echo from server, {}", buffer);
    socket.close();

    server.shutdown();
}

TEST(TestIOCP, Server_1) {
    auto serv_ctx = sese::security::SSLContextBuilder::SSL4Server();
    serv_ctx->importCertFile(PROJECT_PATH "/sese/test/Data/test-ca.crt");
    serv_ctx->importPrivateKeyFile(PROJECT_PATH "/sese/test/Data/test-key.pem");
    ASSERT_TRUE(serv_ctx->authPrivateKey());

    auto address = sese::net::IPv4Address::localhost(sese::net::createRandomPort());

    MyIOCPServer server;
    server.setAddress(address);
    server.setServCtx(serv_ctx);
    server.setThreads(2);
    server.setServProtos("\x8http/1.1");
    ASSERT_TRUE(server.init());
    SESE_INFO("server listening on {}", address->getPort());

    auto client_ctx = sese::security::SSLContextBuilder::SSL4Client();
    auto socket = client_ctx->newSocketPtr(sese::net::Socket::Family::IPv4, 0);
    ASSERT_EQ(socket->connect(address), 0);
    constexpr auto s = "Hello World";
    socket->write(s, strlen(s));

    char buffer[32]{};
    socket->read(buffer, sizeof(buffer));
    SESE_INFO("echo from server, {}", buffer);
    socket->close();

    server.shutdown();
}