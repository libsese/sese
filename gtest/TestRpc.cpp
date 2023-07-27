#include <sese/service/RpcService.h>
#include <sese/net/rpc/Client.h>
#include <sese/service/BalanceLoader.h>
#include <sese/security/SSLContextBuilder.h>
#include <sese/record/Marco.h>
#define REMOTE_API

#include <gtest/gtest.h>

#include <random>

using namespace std::chrono_literals;

sese::net::IPv4Address::Ptr createAddress() {
    std::random_device device;
    auto engine = std::default_random_engine(device());
    std::uniform_int_distribution<uint16_t> dis(1025, 65535);
    auto port = dis(engine);
    printf("select port %d\n", (int) port);
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
    auto serv = new sese::service::RpcService(context);
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
    ASSERT_TRUE(service.init<sese::service::RpcService>(std::bind(&create, servCtx)));

    service.start();
    ASSERT_TRUE(service.isStarted());

    auto client = sese::net::rpc::Client(addr, true);

    int64_t value0 = 1;
    int64_t value1 = 2;
    auto args = std::make_shared<sese::json::ObjectData>();
    SESE_JSON_SET_INTEGER(args, "value0", value0);
    SESE_JSON_SET_INTEGER(args, "value1", value1);
    auto result = client.call("add", args);

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

    std::this_thread::sleep_for(300ms);
    service.stop();
}

TEST(TestRpc, NoSSL) {
    auto addr = createAddress();

    sese::service::BalanceLoader service;
    service.setThreads(2);
    service.setAddress(addr);
    ASSERT_TRUE(service.init<sese::service::RpcService>(std::bind(&create, nullptr)));

    service.start();
    ASSERT_TRUE(service.isStarted());

    auto client = sese::net::rpc::Client(addr);

    int64_t value0 = 1;
    int64_t value1 = 2;
    auto args = std::make_shared<sese::json::ObjectData>();
    SESE_JSON_SET_INTEGER(args, "value0", value0);
    SESE_JSON_SET_INTEGER(args, "value1", value1);
    auto result = client.call("add", args);

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

    std::this_thread::sleep_for(300ms);
    service.stop();
}