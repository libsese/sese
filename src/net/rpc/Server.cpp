#include <sese/net/rpc/Server.h>
#include <sese/config/json/JsonUtil.h>
#include <sese/Packaged2Stream.h>

#include <utility>

using namespace sese;
using namespace sese::json;

void rpc::Server::serve(const IPAddress::Ptr &address, size_t threads) noexcept {
    tcpServer = TcpServer::create(address, threads, 0);

    tcpServer->loopWith([this](IOContext *context) {
        auto stream = std::make_shared<ClosablePackagedStream<IOContext>>(context);
        auto object = json::JsonUtil::deserialize(stream, 5);

        auto nameData = object->getDataAs<BasicData>("rpc-name");
        if (!nameData) return;

        auto name = nameData->getDataAs<std::string>("null");
        if ("null" == name) return;

        auto iterator = map.find(name);
        if (iterator == map.end()) return;

        auto func = iterator->second;
        auto args = object->getDataAs<ObjectData>("rpc-args");
        auto result = std::make_shared<json::ObjectData>();

        func(args, result);

        JsonUtil::serialize(result, stream);
        stream->close();
    });
}

void rpc::Server::enroll(const std::string &name, sese::rpc::Server::Func func) noexcept {
    this->map[name] = std::move(func);
}

void rpc::Server::shutdown() noexcept {
    tcpServer->shutdown();
}