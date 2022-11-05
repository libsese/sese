#include <sese/net/rpc/Server.h>
#define REMOTE_API

REMOTE_API void add(sese::json::ObjectData::Ptr &args, sese::json::ObjectData::Ptr &result) {
    auto value0 = args->getDataAs<sese::json::BasicData>("value0")->getDataAs<int64_t>(0);
    auto value1 = args->getDataAs<sese::json::BasicData>("value1")->getDataAs<int64_t>(0);

    auto temp = std::make_shared<sese::json::BasicData>();
    temp->setDataAs<int64_t>(value0 + value1);
    result->set("add-result", temp);
}

int main() {
    auto address = sese::IPv4Address::create("0.0.0.0", 8080);
    sese::rpc::Server server;
    server.enroll("add", add);
    server.serve(address);
    return 0;
}