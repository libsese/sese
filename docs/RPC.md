# RPC

## 通信格式

sese 内置的 Rpc 服务使用 JSON 作为数据传输格式，基于 TCP 进行通信。

请求示例：

```json
{
    "rpc-ver": "v0.1",
    "rpc-name": "add",
    "rpc-args": {
        "value0": 1,
        "value1": 2
    }
}
```

字段解释：

| 名称     | 注释         |
| -------- | ------------ |
| rpc-ver  | 保留字段     |
| rpc-name | 远程调用名称 |
| rpc-args | 远程参数对象 |

---

响应示例：

```json
{
    "rpc-ver": "v0.1",
    "rpc-exit-code": 0,
    "add-result": 3
}
```

| 名称          | 注释       |
| ------------- | ---------- |
| rpc-ver       | Rpc 版本   |
| rpc-exit-code | 退出代码   |
| add-result    | 自定义字段 |

## 退出码

其中 0~255 为保留错误号

| 退出码 | 注释                                               | 宏                                    |
| ------ | -------------------------------------------------- | ------------------------------------- |
| 0      | 无错误退出                                         | SESE_RPC_CODE_SUCCEED                 |
| 1      | 协议版本不支持                                     | SESE_RPC_CODE_NONSUPPORT_VERSION      |
| 2      | 缺少必要 JSON 字段，例如：rpc-name、rpc-version 等 | SESE_RPC_CODE_MISSING_REQUIRED_FIELDS |
| 3      | 不存在远程调用过程                                 | SESE_RPC_CODE_NO_EXIST_FUNC           |
| 4      | 远程调用参数错误                                   | SESE_RPC_CODE_ILLEGAL_ARGS            |

## 服务端示例

```c++
#include <sese/net/rpc/Server.h>
#include <sese/thread/Thread.h>

void add(sese::json::ObjectData::Ptr &args, sese::json::ObjectData::Ptr &result) {
    GetInteger4Server(value0, "value0", 0);
    GetInteger4Server(value1, "value1", 0);

    SetInteger(result, "add-result", value0 + value1);
    SetBoolean(result, "is-homo", true);
    SetString(result, "msg", "94A");
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
```

## 客户端示例

```c++
#include <sese/net/rpc/Server.h>

int main() {
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
        printf("remote call was succeed, add-result: %d\n", (int)add_result);
        printf("is-homo: %s\n", is_homo ? "true" : "false");
        printf("msg: %s\n", msg.c_str());
    } else {
        puts("remote call was failed");
    }
    return 0;
}
```

