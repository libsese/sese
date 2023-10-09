#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(disable : 4996)
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include "sese/net/rpc/V2RpcServer.h"
#include "sese/net/rpc/Marco.h"
#include "sese/config/json/JsonUtil.h"
#include "sese/io/FakeStream.h"
#include "sese/io/BufferedInputStream.h"
#include "sese/io/BufferedOutputStream.h"

using namespace sese;
using namespace sese::json;
using namespace sese::net::v2::rpc;

#define BuiltinSetExitCode(code) exit->setDataAs<int64_t>(code)

void V2RpcServer::onHandle(sese::net::v2::IOContext &ctx) noexcept {
    auto stream = std::make_shared<io::ClosableFakeStream<IOContext>>(&ctx);

    auto input = std::make_shared<io::BufferedInputStream>(stream);
    auto object = json::JsonUtil::deserialize(input, 5);
    if (!object) return; // 序列化请求失败
    auto result = std::make_shared<json::ObjectData>();

    auto exit = std::make_shared<json::BasicData>();
    BuiltinSetExitCode(SESE_RPC_CODE_SUCCEED);
    result->set(SESE_RPC_TAG_EXIT_CODE, exit);

    auto version = std::make_shared<json::BasicData>();
    version->setDataAs<std::string>(SESE_RPC_VERSION_0_1);
    result->set(SESE_RPC_TAG_VERSION, version);

    // 1.版本确定
    auto output = std::make_shared<io::BufferedOutputStream>(stream);
    std::string ver;
    auto verData = object->getDataAs<BasicData>(SESE_RPC_TAG_VERSION);
    if (nullptr == verData) {
        BuiltinSetExitCode(SESE_RPC_CODE_MISSING_REQUIRED_FIELDS);
        JsonUtil::serialize(result, output);
        output->flush();
        stream->close();
        return;
    } else {
        ver = verData->getDataAs<std::string>(SESE_RPC_VALUE_UNDEF);
    }

    if (SESE_RPC_VALUE_UNDEF == ver) {
        BuiltinSetExitCode(SESE_RPC_CODE_MISSING_REQUIRED_FIELDS);
        JsonUtil::serialize(result, output);
        output->flush();
        stream->close();
        return;
    } else if (SESE_RPC_VERSION_0_1 != ver) {
        BuiltinSetExitCode(SESE_RPC_CODE_NONSUPPORT_VERSION);
        JsonUtil::serialize(result, output);
        output->flush();
        stream->close();
        return;
    }

    // 2.获取并检查远程调用名称
    std::string name;
    Get4Server(name, object, result, std::string, SESE_RPC_TAG_NAME, SESE_RPC_VALUE_UNDEF);
    if (SESE_RPC_VALUE_UNDEF == name) {
        BuiltinSetExitCode(SESE_RPC_CODE_MISSING_REQUIRED_FIELDS);
        JsonUtil::serialize(result, output);
        output->flush();
        stream->close();
        return;
    }

    auto iterator = map.find(name);
    if (iterator == map.end()) {
        BuiltinSetExitCode(SESE_RPC_CODE_NO_EXIST_FUNC);
        JsonUtil::serialize(result, output);
        output->flush();
        stream->close();
        return;
    }

    // 3.获取并检查用户过程
    auto func = iterator->second;
    auto args = object->getDataAs<ObjectData>(SESE_RPC_TAG_ARGS);
    if (!args) {
        BuiltinSetExitCode(SESE_RPC_CODE_MISSING_REQUIRED_FIELDS);
        JsonUtil::serialize(result, output);
        output->flush();
        stream->close();
        return;
    }

    // 4.执行用户过程
    func(args, result);

    // 5.序列化
    JsonUtil::serialize(result, output);
    output->flush();
    stream->close();
}

void V2RpcServer::setFunction(const std::string &name, const V2RpcServer::Func &func) noexcept {
    map[name] = func;
}

#undef BuiltinSetExitCode

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif