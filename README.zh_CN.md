# README

<h1 align="center">Sese Framework</h1>
<div align="center">
<img src="static/image/logo.svg" width="128" height="128" alt="logo"/>
</div>
<div align="center">
<img src="https://img.shields.io/static/v1?label=license&message=Apache-2.0&color=purple&logo=Apache" alt="license"/>
<img src="https://img.shields.io/static/v1?label=language&message=C%2B%2B%2020&color=purple&logo=cplusplus" alt="lang"/>
<img src="https://img.shields.io/static/v1?label=build%20system&message=CMake&color=purple&logo=cmake" alt="buildsystem"/>
<br>
<img alt="CodeFactor Status" src="https://www.codefactor.io/repository/github/libsese/sese/badge"/>
<img alt="Lines of Code" src="https://sloc.xyz/github/libsese/sese/"/>
<img alt="GitHub Workflow Status (with event)" src="https://img.shields.io/github/actions/workflow/status/libsese/sese/windows-2022.yml?label=Windows&logo=windows">
<img alt="GitHub Workflow Status (with event)" src="https://img.shields.io/github/actions/workflow/status/libsese/sese/ubuntu-2204-apt.yml?label=Ubuntu&logo=ubuntu">
<img alt="GitHub Workflow Status (with event)" src="https://img.shields.io/github/actions/workflow/status/libsese/sese/macos-12-brew.yml?label=macOS&logo=apple">
<br>
</div>

## 简介

这是一个跨平台的、用于基础组件开发的框架，一定程度上作为标准库的补充使用。定位上类似于 `Boost`、`folly` 之于标准库。项目使用
C++ 20 标准，引入了 vcpkg 作为包管理器帮助我们简化依赖项管理问题。

## 示例

内建日志器

现代的格式化方式

```c++
#include <sese/Log.h>
// ...
namespace sese::text::overload {
template<>
struct Formatter<Point> {
    bool parse(const std::string &) { return true; }
    void format(FmtCtx &ctx, const Point &p) {
        ctx.builder << fmt("({},{})", p.x, p.y);
    }
};
}
// ...
int a = 1, b = 2;
Point point{1, 2};
SESE_INFO("hello world");
SESE_INFO("a + b = {}", a + b);
SESE_INFO("point {}", point);
```

> 2024-06-27T01:43:05.571Z I main.cpp:7 Main:10376> hello world<br>
> 2024-06-27T01:43:05.572Z I main.cpp:8 Main:10376> a + b = 3<br>
> 2024-06-27T01:43:05.572Z I main.cpp:9 Main:10376> point (1,2)

类 C 的格式化方式

```c++
#define SESE_C_LIKE_FORMAT
#include <sese/Log.h>
// ...
SESE_INFO("hello world");
SESE_WARN("error %s", err.what().c_str());
```

> 2024-05-15T15:54:48.296Z I main.cpp:8 Main:7116> hello world<br>
> 2024-05-15T15:54:48.296Z W main.cpp:9 Main:7116> error End of file

HTTP 控制器

```c++
#include <sese/service/http/HttpServer_V3.h>
// ...
SESE_CTRL(MyController, std::mutex mutex{}; int times = 0) {
    SESE_INFO("LOADING MyController");
    SESE_URL(timers, RequestType::GET, "/times") {
        sese::Locker locker(mutex);
        times += 1;
        auto message = "timers = '" + std::to_string(this->times) + "'\n";
        resp.getBody().write(message.data(), message.length());
    };
    SESE_URL(say, RequestType::GET, "/say?<say>") {
        auto words = req.get("say");
        auto message = "you say '" + words + "'\n";
        resp.getBody().write(message.data(), message.length());
    };
    SESE_INFO("LOADED");
}
```

跨进程通讯

```c++
#include <sese/system/IPC.h>
#include <sese/Log.h>
// ···
// server
auto channel = sese::system::IPCChannel::create("Test", 1024);
while (true) {
    auto messages = channel->read(1);
    if (messages.empty()) {
        sese::sleep(1s);
        continue;
    }
    for (auto &&msg: messages) {
        SESE_INFO("recv {}", msg.getDataAsString());
        if (msg.getDataAsString() == "Exit") {
            goto end;
        }
    }
}
end:
return 0;
// ···
// client
auto channel = sese::system::IPCChannel::use("Test");
channel->write(1, "Hello");
channel->write(2, "Hi");
channel->write(1, "Exit");
```

## 构建

### 对于开发者/贡献者

1. 配置开发环境

对于 Windows 用户，请安装并配置 [vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started) 到目标机器上。

对于非 Windows 用户，vcpkg 同样是可用的。但同时你可以选择使用原生的系统依赖管理工具安装相应依赖。我们提供了几个预设的安装脚本。

- Ubuntu

```bash
sudo ./scripts/install_ubuntu_deps.sh
```

- Fedora

```bash
sudo ./scripts/install_fedora_deps.sh
```

- macOS

```bash
./scripts/install_darwin_deps.sh
```

2. 编译选项

如果你配置好了 vcpkg，只需[设置工具链文件](https://learn.microsoft.com/en-us/vcpkg/users/buildsystems/cmake-integration)
即可完成依赖配置。

如果你使用系统依赖管理工具，那么你需要在按完成对应依赖后，手动添加 `SESE_USE_NATIVE_MANAGER` 编译选项。

我们定义了一些 options 用于启用或关闭部分功能，具体可以参照以下的类似格式：

https://github.com/libsese/sese/blob/4cd74389d7105b71c632070c775a727be8ee413d/CMakeLists.txt#L8-L16

> [!TIP]
> 请根据最新 [CMakeLists.txt](./CMakeLists.txt) 文件了解相应的功能选项，
> 或者你可以直接使用我们预设的 [CMakePresets.json](./CMakePresets.json), 这将默认开启绝大多数功能选项。

3. 编译

配置完成编译选项只需要常规构建即可，例如：

```bash
cmake --build build/linux-debug -- -j 4
```

### 对于普通使用者

对于普通使用者，我们推荐使用 vcpkg 导入此依赖，可以参考我们的[模板项目](https://github.com/libsese/sese-template)配置你的项目：

> [!WARNING]
> 项目也可以作为普通项目安装在普通机器上，但这不是推荐的做法，也无法得到支持，
> 如果你想这么做可以参考 `构建` > `对于开发者/贡献者` 中关于系统依赖管理工具的内容。

主要工作是编写项目的依赖配置文件，例如：

`vcpkg.json`

```json
{
  "dependencies": [
    "sese"
  ]
}
```

> [!IMPORTANT]
> 自内建库基线 `14b91796a68c87bc8d5cb35911b39287ccb7bd95` 之后，sese 已进入内建列表。
> 在此之前，你需要多编写一个配置文件用于导入我们的[私有注册表](https://learn.microsoft.com/en-us/vcpkg/consume/git-registries)，就像这样：

`vcpkg-configuration.json`

```json
{
  "default-registry": {
    "kind": "git",
    "repository": "https://github.com/microsoft/vcpkg.git",
    "baseline": "c8696863d371ab7f46e213d8f5ca923c4aef2a00"
  },
  "registries": [
    {
      "kind": "git",
      "repository": "https://github.com/libsese/vcpkg-registry.git",
      "baseline": "73268778d5f796f188ca66f71536377b171ee37e",
      "packages": [
        "sese"
      ]
    }
  ]
}
```

如果你没有使用 vcpkg，上述步骤是不必要的。

## 测试

使用了 googletest 作为了测试框架，测试的详细信息可以从 github actions 中查看，包括各个平台的测试结果和 linux 的测试覆盖率等内容。

| 平台 | 入口 | 单元测试 | 覆盖率测试 |
|----|----|----|----|
| Windows | [Unit Tests](https://github.com/libsese/sese/actions/workflows/windows-2022.yml) | ✅ | |
| Linux   | [Unit Tests](https://github.com/libsese/sese/actions/workflows/ubuntu-22.04-apt.yml) | ✅ | ✅ |
| macOS   | [Unit Tests](https://github.com/libsese/sese/actions/workflows/macos-12-brew.yml) | ✅ | |

1. 本地测试

如果你需要在本地运行完整测试，那么你可能需要注意关于数据库方面的测试，这需要一些额外的服务和配置支撑。

- Ubuntu 工作流对于服务的处理参考

https://github.com/libsese/sese/blob/4cd74389d7105b71c632070c775a727be8ee413d/.github/workflows/ubuntu-22.04-apt.yml#L14-L34

https://github.com/libsese/sese/blob/4cd74389d7105b71c632070c775a727be8ee413d/.github/workflows/ubuntu-22.04-apt.yml#L45-L53

https://github.com/libsese/sese/blob/4cd74389d7105b71c632070c775a727be8ee413d/.github/workflows/ubuntu-22.04-apt.yml#L72-L75

- 直接使用 docker-compose.yml

```bash
docker-compose -f ./docker/docker-compose.yml up -d
sqlite3 build/db_test.db < scripts/sqlite_dump.sql
```

2. 生成覆盖率测试报告

需要安装 gcovr，可以选择 pip 安装或者使用系统包管理器安装。

```bash
mkdir -p build/coverage/html
gcovr
```

> [!NOTE]
> 生成覆盖率测试数据首先需要设置一些额外的编译选项来生成例如 gcov 格式的测试数据，例如 GCC：

https://github.com/libsese/sese/blob/4cd74389d7105b71c632070c775a727be8ee413d/.github/workflows/ubuntu-22.04-apt.yml#L75

## 文档

[文档](https://libsese.github.io/sese/)将会随着主线的更新而自动更新到 github pages 上。

文档内容将从代码注释中自动生成，docs 目录实际存放构建文档所需的部分资源。

## 贡献者

![Contributors](https://contrib.rocks/image?repo=libsese/sese)