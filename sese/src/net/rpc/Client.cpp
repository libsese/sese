#include <sese/net/rpc/Client.h>
#include <sese/net/AddressPool.h>
#include <sese/text/StringBuilder.h>
#include <sese/security/SSLContextBuilder.h>
#include <sese/config/json/JsonUtil.h>
#include <sese/util/Util.h>

using namespace sese;
using namespace sese::json;
using namespace sese::security;
using namespace sese::net::rpc;

Client::Client(const IPv4Address::Ptr &address, bool ssl, const std::string &version) noexcept {
    if (ssl) {
        this->sslContext = SSLContextBuilder::SSL4Client();
    }
    this->address = address;
    this->version = std::make_shared<json::BasicData>();
    this->version->setDataAs<std::string>(version);
}

// Client::Client(const IPv4Address::Ptr &address, const std::string &version) noexcept {
//     this->address = address;
//     this->version = std::make_shared<json::BasicData>();
//     this->version->setDataAs<std::string>(version);
// }

Client::~Client() noexcept {
    if (socket) {
        socket->close();
        socket = nullptr;
    }
}

json::ObjectData::Ptr Client::makeTemplateRequest(const std::string &name) {
    auto object = std::make_shared<json::ObjectData>();
    object->set(SESE_RPC_TAG_VERSION, version);
    SESE_JSON_SET(object, std::string, SESE_RPC_TAG_NAME, name);
    return object;
}

bool Client::reconnect() noexcept {
    if (socket) {
        socket->close();
    }

    if (this->sslContext) {
        this->socket = this->sslContext->newSocketPtr(Socket::Family::IPv4, IPPROTO_IP);
    } else {
        this->socket = std::make_shared<Socket>(Socket::Family::IPv4, Socket::Type::TCP, IPPROTO_IP);
    }

    if (0 != this->socket->connect(this->address)) {
        this->socket = nullptr;
        return false;
    } else {
        return true;
    }
}

json::ObjectData::Ptr Client::doRequest(const std::string &name, json::ObjectData::Ptr &args) noexcept {
    auto object = makeTemplateRequest(name);
    object->set(SESE_RPC_TAG_ARGS, args);
    buffer.freeCapacity();
    json::JsonUtil::serialize(object.get(), &buffer);

    if (!reconnect()) {
        return nullptr;
    }

    char buf[MTU_VALUE];
    while (true) {
        auto len = buffer.peek(buf, MTU_VALUE);
        if (len == 0) {
            break;
        }
        auto l = socket->write(buf, len);
        if (l < 0) {
            return nullptr;
        } else {
            buffer.trunc(l);
        }
    }

    buffer.freeCapacity();
    auto result = json::JsonUtil::deserialize(socket.get(), 5);
    return result;
}

#define RETURN(TYPE)           \
    case SESE_RPC_CODE_##TYPE: \
        return SESE_RPC_VALUE_##TYPE

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