#include <sese/service/rpc/RpcService_V1.h>
#include <sese/net/rpc/Client.h>
#include <sese/service/BalanceLoader.h>
#include <sese/security/SSLContextBuilder.h>
#include <sese/record/Marco.h>
#include <sese/config/json/JsonUtil.h>
#include <sese/io/OutputUtil.h>
#include <sese/util/Util.h>
#include <sese/thread/Async.h>

#define REMOTE_API
#define printf SESE_INFO

#include <gtest/gtest.h>

#include <random>

using namespace std::chrono_literals;

static sese::net::IPv4Address::Ptr createAddress() {
    auto port = sese::net::createRandomPort();
    printf("select port %d", (int) port);
    return sese::net::IPv4Address::create("127.0.0.1", port);
}

REMOTE_API void add(sese::json::ObjectData::Ptr &args, sese::json::ObjectData::Ptr &result) {
    GetInteger4Server(value0, "value0", 0);
    GetInteger4Server(value1, "value1", 0);

    SESE_JSON_SET_INTEGER(result, "add-result", value0 + value1);
    SESE_JSON_SET_BOOLEAN(result, "is-homo", true);
    SESE_JSON_SET_STRING(result, "msg", "94A");
}

auto create(const sese::security::SSLContext::Ptr &context) {
    auto serv = new sese::service::v1::RpcService(context);
    serv->setFunction("add", add);
    return serv;
}

TEST(TestRpc, WithSSL) {
    auto servCtx = sese::security::SSLContextBuilder::SSL4Server();
    servCtx->importCertFile(PROJECT_PATH "/gtest/Data/test-ca.crt");
    servCtx->importPrivateKeyFile(PROJECT_PATH "/gtest/Data/test-key.pem");
    ASSERT_TRUE(servCtx->authPrivateKey());

    auto addr = createAddress();

    sese::service::BalanceLoader service;
    service.setThreads(2);
    service.setAddress(addr);
    ASSERT_TRUE(service.init<sese::service::v1::RpcService>(std::bind(&create, servCtx)));

    service.start();
    ASSERT_TRUE(service.isStarted());

    auto client = sese::net::rpc::Client(addr, true);
    sese::json::ObjectData::Ptr args;
    sese::json::ObjectData::Ptr result;
    {
        args = std::make_shared<sese::json::ObjectData>();
        int64_t value0 = 1;
        int64_t value1 = 2;
        SESE_JSON_SET_INTEGER(args, "value0", value0);
        SESE_JSON_SET_INTEGER(args, "value1", value1);
        result = client.doRequest("add", args);
        ASSERT_NE(result, nullptr);

        SESE_JSON_GET_INTEGER(code, result, SESE_RPC_TAG_EXIT_CODE, 0);
        ASSERT_EQ(SESE_RPC_CODE_SUCCEED, code) << sese::net::rpc::getErrorMessage(code);

        SESE_JSON_GET_INTEGER(add_result, result, "add-result", 0);
        SESE_JSON_GET_BOOLEAN(is_homo, result, "is-homo", false);
        SESE_JSON_GET_STRING(msg, result, "msg", "undef");

        EXPECT_EQ(add_result, 3);
        EXPECT_EQ(is_homo, true);
        EXPECT_EQ(msg, "94A");

        SESE_INFO("remote call was succeed, add-result: %d", (int) add_result);
        SESE_INFO("is-homo: %s", is_homo ? "true" : "false");
        SESE_INFO("msg: %s", msg.c_str());
    }

    {
        args = std::make_shared<sese::json::ObjectData>();
        result = client.doRequest("dev-0", args);
        ASSERT_NE(result, nullptr);

        SESE_JSON_GET_INTEGER(code, result, SESE_RPC_TAG_EXIT_CODE, 0);
        ASSERT_EQ(SESE_RPC_CODE_NO_EXIST_FUNC, code) << sese::net::rpc::getErrorMessage(code);
    }

    {
        args = std::make_shared<sese::json::ObjectData>();
        SESE_JSON_SET_STRING(args, "str", "\"");
        result = client.doRequest("dev-1", args);

        ASSERT_EQ(result, nullptr);
    }

    {
        auto client_0 = sese::net::rpc::Client(addr, true, "0");
        args = std::make_shared<sese::json::ObjectData>();

        result = client_0.doRequest("dev-2", args);
        ASSERT_NE(result, nullptr);

        SESE_JSON_GET_INTEGER(code, result, SESE_RPC_TAG_EXIT_CODE, 0);
        ASSERT_EQ(SESE_RPC_CODE_NONSUPPORT_VERSION, code) << sese::net::rpc::getErrorMessage(code);
    }

    {
        auto clientCtx = sese::security::SSLContextBuilder::SSL4Client();
        auto socket = clientCtx->newSocketPtr(sese::net::Socket::Family::IPv4, 0);
        socket->connect(addr);
        socket->shutdown(sese::net::Socket::ShutdownMode::Both);
        socket->close();
    }

    {
        const char *buffer = {"{\n"
                              "  \"rpc-args\": {\n"
                              "    \"value0\": 1,\n"
                              "    \"value1\": 2\n"
                              "  },\n"
                              "  \"rpc-name\": \"add\"\n"
                              "}"};
        auto clientCtx = sese::security::SSLContextBuilder::SSL4Client();
        auto socket = clientCtx->newSocketPtr(sese::net::Socket::Family::IPv4, 0);
        socket->connect(addr);
        *socket << buffer;

        result = sese::json::JsonUtil::deserialize(socket.get(), 5);
        ASSERT_NE(result, nullptr);
        SESE_JSON_GET_INTEGER(code, result, SESE_RPC_TAG_EXIT_CODE, 0);
        ASSERT_EQ(SESE_RPC_CODE_MISSING_REQUIRED_FIELDS, code) << sese::net::rpc::getErrorMessage(code);

        socket->shutdown(sese::net::Socket::ShutdownMode::Both);
        socket->close();
    }

    {
        const char *buffer = {"{\n"
                              "  \"rpc-args\": {\n"
                              "    \"value0\": 1,\n"
                              "    \"value1\": 2\n"
                              "  },\n"
                              "  \"rpc-ver\": \"v0.1\"\n"
                              "}"};
        auto clientCtx = sese::security::SSLContextBuilder::SSL4Client();
        auto socket = clientCtx->newSocketPtr(sese::net::Socket::Family::IPv4, 0);
        socket->connect(addr);
        *socket << buffer;

        result = sese::json::JsonUtil::deserialize(socket.get(), 5);
        ASSERT_NE(result, nullptr);
        SESE_JSON_GET_INTEGER(code, result, SESE_RPC_TAG_EXIT_CODE, 0);
        ASSERT_EQ(SESE_RPC_CODE_MISSING_REQUIRED_FIELDS, code) << sese::net::rpc::getErrorMessage(code);

        socket->shutdown(sese::net::Socket::ShutdownMode::Both);
        socket->close();
    }

    {
        const char *buffer = {"{\n"
                              "  \"rpc-name\": \"add\",\n"
                              "  \"rpc-ver\": \"v0.1\"\n"
                              "}"};
        auto clientCtx = sese::security::SSLContextBuilder::SSL4Client();
        auto socket = clientCtx->newSocketPtr(sese::net::Socket::Family::IPv4, 0);
        socket->connect(addr);
        *socket << buffer;

        result = sese::json::JsonUtil::deserialize(socket.get(), 5);
        ASSERT_NE(result, nullptr);
        SESE_JSON_GET_INTEGER(code, result, SESE_RPC_TAG_EXIT_CODE, 0);
        ASSERT_EQ(SESE_RPC_CODE_MISSING_REQUIRED_FIELDS, code) << sese::net::rpc::getErrorMessage(code);

        socket->shutdown(sese::net::Socket::ShutdownMode::Both);
        socket->close();
    }

    std::this_thread::sleep_for(300ms);
    service.stop();
}

TEST(TestRpc, NoSSL) {
    auto addr = createAddress();

    sese::service::BalanceLoader service;
    service.setThreads(2);
    service.setAddress(addr);
    ASSERT_TRUE(service.init<sese::service::v1::RpcService>(std::bind(&create, nullptr)));

    service.start();
    ASSERT_TRUE(service.isStarted());

    auto client = sese::net::rpc::Client(addr);

    int64_t value0 = 7;
    int64_t value1 = 8;
    auto args = std::make_shared<sese::json::ObjectData>();
    SESE_JSON_SET_INTEGER(args, "value0", value0);
    SESE_JSON_SET_INTEGER(args, "value1", value1);

    auto future = sese::async<sese::json::ObjectData::Ptr>([&]() {
        return client.doRequest("add", args);
    });

    std::future_status status{};
    do {
        SESE_INFO("no ready");
        status = future.wait_for(100ms);
    } while (status != std::future_status::ready);

    const auto &result = future.get();
    ASSERT_NE(result, nullptr);

    SESE_JSON_GET_INTEGER(code, result, SESE_RPC_TAG_EXIT_CODE, 0);
    ASSERT_EQ(SESE_RPC_CODE_SUCCEED, code) << sese::net::rpc::getErrorMessage(code);

    SESE_JSON_GET_INTEGER(add_result, result, "add-result", 0);
    SESE_JSON_GET_BOOLEAN(is_homo, result, "is-homo", false);
    SESE_JSON_GET_STRING(msg, result, "msg", "undef");

    EXPECT_EQ(add_result, 15);
    EXPECT_EQ(is_homo, true);
    EXPECT_EQ(msg, "94A");

    SESE_INFO("remote call was succeed, add-result: %d", (int) add_result);
    SESE_INFO("is-homo: %s", is_homo ? "true" : "false");
    SESE_INFO("msg: %s", msg.c_str());

    auto socket = sese::net::Socket(sese::net::Socket::Family::IPv4, sese::net::Socket::Type::TCP);
    socket.connect(addr);
    socket.shutdown(sese::net::Socket::ShutdownMode::Both);
    socket.close();

    std::this_thread::sleep_for(100ms);
    service.stop();
}