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

## 操作宏

### 一览

| 宏                | 注释                                |
| ----------------- | ----------------------------------- |
| SetExitCode       | 用于快速设置 Rpc 退出码             |
| GetBoolean4Server | 服务端快速获取 bool 类型字段        |
| GetInteger4Server | 服务端快速获取 int64_t 类型字段     |
| GetDouble4Server  | 服务端快速获取 double 类型字段      |
| GetString4Server  | 服务端快速获取 std::string 类型字段 |
| GetBoolean        | 快速获取 bool 类型字段              |
| GetInteger        | 快速获取 int64_t 类型字段           |
| GetDouble         | 快速获取 double 类型字段            |
| GetString         | 快速获取 std::string 类型字段       |

具体参数请参考代码及其注释

### Get \${Type} 和 Get \${Type} 4Server 的区别

前者仅用于快速获取一个字段，而后者获取字段的同时能快速处理非法的参数，通常用于服务端。

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
    auto server = sese::rpc::Server::create(address);
    if (nullptr == server) {
        puts("failed to start the rpc service");
        exit(-1);
    }

    server->enroll("add", add);

    sese::Thread thread([&server](){
        server->serve();
    });
    thread.start();
    getchar();
    server->shutdown();
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

