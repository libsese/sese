#include "sese/net/rpc/Marco.h"
#include "sese/net/rpc/V2RpcServerOption.h"
#include "sese/util/Packaged2Stream.h"
#include "sese/util/BufferedInputStream.h"
#include "sese/util/BufferedOutputStream.h"

using namespace sese;
using namespace sese::json;

#define BuiltinSetExitCode(code) exit->setDataAs<int64_t>(code)

void sese::net::v2::rpc::V2RpcServerOption::onHandle(sese::net::v2::IOContext *ctx) noexcept {
    auto stream = std::make_shared<ClosablePackagedStream<IOContext>>(ctx);

    auto input = std::make_shared<BufferedInputStream>(stream);
    auto object = json::JsonUtil::deserialize(input, 5);
    if (!object) return;// 序列化请求失败
    auto result = std::make_shared<json::ObjectData>();

    auto exit = std::make_shared<json::BasicData>();
    BuiltinSetExitCode(SESE_RPC_CODE_SUCCEED);
    result->set(SESE_RPC_TAG_EXIT_CODE, exit);

    auto version = std::make_shared<json::BasicData>();
    version->setDataAs<std::string>(SESE_RPC_VERSION_0_1);
    result->set(SESE_RPC_TAG_VERSION, version);

    // 1.版本确定
    auto output = std::make_shared<BufferedOutputStream>(stream);
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

#undef BuiltinSetExitCode