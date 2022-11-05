#include <sese/net/rpc/Server.h>
#include <sese/thread/Thread.h>
#define REMOTE_API

REMOTE_API void add(sese::json::ObjectData::Ptr &args, sese::json::ObjectData::Ptr &result) {
    GetInteger(value0, "value0", 0);
    GetInteger(value1, "value1", 0);

    auto temp = std::make_shared<sese::json::BasicData>();
    temp->setDataAs<int64_t>(value0 + value1);
    result->set("add-result", temp);
}

int main() {
    auto address = sese::IPv4Address::create("0.0.0.0", 8080);
    sese::rpc::Server server;
    server.enroll("add", add);

    sese::Thread thread([&server, address](){
        server.serve(address);
    });
    thread.start();
    getchar();
    server.shutdown();
    thread.join();
    return 0;
}