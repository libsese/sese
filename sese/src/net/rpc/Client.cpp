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

    this->socket->setNonblocking();

    if (0 != this->socket->connect(this->address)) {
        this->socket = nullptr;
        return false;
    } else {
        return true;
    }
}

bool Client::doRequest(const std::string &name, json::ObjectData::Ptr &args) noexcept {
    auto object = makeTemplateRequest(name);
    object->set(SESE_RPC_TAG_ARGS, args);
    buffer.freeCapacity();
    json::JsonUtil::serialize(object.get(), &buffer);

    if (!reconnect()) {
        return false;
    }

    char buf[MTU_VALUE];
    while (true) {
        auto len = buffer.peek(buf, MTU_VALUE);
        if (len == 0) {
            return true;
        }
        auto l = socket->write(buf, len);
        if (l <= 0) {
            auto err = sese::net::getNetworkError();
            if (err == EWOULDBLOCK || err == EINTR || err == ECONNABORTED || err == 0) {
                sese::sleep(0);
                continue;
            } else {
                // 断开连接...等
                return false;
            }
        } else {
            buffer.trunc(l);
        }
    }
}

json::ObjectData::Ptr Client::doResponse() noexcept {
    if (socket) {
        buffer.freeCapacity();
        char buf[MTU_VALUE];
        while (true) {
            auto l = socket->read(buf, MTU_VALUE);
            if (l <= 0) {
                auto err = sese::net::getNetworkError();
                if (err == ENOTCONN) {
                    // 断开连接
                    return nullptr;
                } else {
                    break;
                }
            } else {
                buffer.write(buf, l);
            }
        }
        auto result = json::JsonUtil::deserialize(&buffer, 5);
        return result;
    }
    return nullptr;
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