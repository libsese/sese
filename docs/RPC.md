# RPC Usage

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
| rpc-ver       | 保留字段   |
| rpc-exit-code | 退出代码   |
| add-result    | 自定义字段 |

