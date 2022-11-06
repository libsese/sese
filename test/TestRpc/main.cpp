#include <sese/net/rpc/Server.h>
#include <sese/thread/Thread.h>
#include <sese/Util.h>
#define REMOTE_API

REMOTE_API void add(sese::json::ObjectData::Ptr &args, sese::json::ObjectData::Ptr &result) {
    GetInteger4Server(value0, "value0", 0);
    GetInteger4Server(value1, "value1", 0);

    SetInteger(result, "add-result", value0 + value1);
    SetBoolean(result, "is-homo", true);
    SetString(result, "msg", "94A");
}

int main() {
    auto address = sese::IPv4Address::create("127.0.0.1", 8080);
    auto server = sese::rpc::Server::create(address);
    if (nullptr == server) {
        puts("failed to start the rpc service");
        exit(-1);
    }

    server->enroll("add", add);

    sese::Thread thread([&server, address]() {
        server->serve();
    });
    thread.start();

    sese::sleep(2);

    int64_t value0 = 114;
    int64_t value1 = 514;

    sese::rpc::Client client(address);
    auto args = std::make_shared<sese::json::ObjectData>();
    SetInteger(args, "value0", value0);
    SetInteger(args, "value1", value1);
    auto result = client.call("add", args);

    GetInteger(code, result, SESE_RPC_TAG_EXIT_CODE, 0);
    if (SESE_RPC_CODE_SUCCEED == code) {
        GetInteger(add_result, result, "add-result", 0);
        GetBoolean(is_homo, result, "is-homo", false);
        GetString(msg, result, "msg", "undef");
        printf("remote call was succeed, add-result: %d\n", (int) add_result);
        printf("is-homo: %s\n", is_homo ? "true" : "false");
        printf("msg: %s\n", msg.c_str());
    } else {
        puts("remote call was failed");
    }

    server->shutdown();
    thread.join();
    return 0;
}