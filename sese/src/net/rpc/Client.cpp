#include <sese/net/rpc/Client.h>
#include <sese/net/Socket.h>
#include <sese/config/json/JsonUtil.h>

using namespace sese;
using namespace sese::json;
using namespace sese::net::rpc;

Client::Client(const IPv4Address::Ptr &address, const std::string &version) noexcept {
    this->address = address;
    this->version = std::make_shared<json::BasicData>();
    this->version->setDataAs<std::string>(version);
}

json::ObjectData::Ptr Client::makeTemplateRequest(const std::string &name) {
    auto object = std::make_shared<json::ObjectData>();
    object->set(SESE_RPC_TAG_VERSION, version);
    Set(object, std::string, SESE_RPC_TAG_NAME, name);
    return object;
}

json::ObjectData::Ptr Client::call(const std::string &name, json::ObjectData::Ptr &args) noexcept {
    auto connect = std::make_shared<Socket>(Socket::Family::IPv4, Socket::Type::TCP);
    if (0 != connect->connect(address)) return nullptr;
    auto object = makeTemplateRequest(name);
    object->set(SESE_RPC_TAG_ARGS, args);
    json::JsonUtil::serialize(object, connect);
    auto result = json::JsonUtil::deserialize(connect, 5);
    return result;
}

#define RETURN(TYPE)           \
    case SESE_RPC_CODE_##TYPE: \
        return SESE_RPC_VALUE_##TYPE;

const char *sese::net::rpc::getErrorMessage(int64_t code) noexcept {
    switch (code) {
        RETURN(SUCCEED);
        RETURN(NONSUPPORT_VERSION);
        RETURN(MISSING_REQUIRED_FIELDS);
        RETURN(NO_EXIST_FUNC);
        RETURN(ILLEGAL_ARGS);
        default:
            return SESE_RPC_VALUE_UNKNOWN;
    }
}

#undef RETURN