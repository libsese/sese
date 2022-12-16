# Sese Library

<div>
  <img src="https://img.shields.io/static/v1?label=license&message=Apache-2.0&color=red"/>
  <img src="https://img.shields.io/static/v1?label=language&message=C%2B%2B%2020&color=greed&logo=cplusplus"/>
  <img src="https://img.shields.io/static/v1?label=build%20system&message=CMake&color=greed&logo=cmake"/>
  <br>
  <img src="https://img.shields.io/static/v1?label&message=windows&color=blue&logo=windows"/>
  <img src="https://img.shields.io/static/v1?label&message=linux&color=blue&logo=linux"/>
  <img src="https://img.shields.io/static/v1?label&message=macOS&color=blue&logo=apple"/>
  <br>
  <a href="mailto://shiina_kaoru@outlook.com">
  <img src="https://img.shields.io/static/v1?label=email&message=SHIINA_KAORU@Outlook.com&color=skyblue&logo=gmail"/>
  </a>
  <img src="https://img.shields.io/static/v1?label=QQ&message=995602964&color=skyblue&logo=tencentqq"/>
</div>

## 基本信息

Sese 是一个支持 Windows、Linux 和 macOS 的跨平台基础库，<br>
跨平台支持以源码级跨平台的方式提供。<br>
设计初衷是为了在保持一定的性能、不变更当前技术栈的情况下，<br>
提供一个开箱即用的 C++ 开发框架。<br>
支持以静态链接或动态链接的方式部署至解决方案中。

注意：框架不支持交叉编译，且仅支持 AMD64 架构的机器。

## 构建

项目选择了 CMake 作为构建系统，同时还使用了 Git 作为项目的版本控制系统（废话）。<br>
其中，强烈推荐您使用 CMake 3.14 以上的版本，使用 FetchContent 为您导入项目依赖。

关于依赖，目前项目引入的依赖主要有以下：

| 名称                  | 地址                                       | 描述               | 父项      | 状态  |
|---------------------|------------------------------------------|------------------|---------|-----|
| SString             | github.com/shiinasama/SString            | UTF-8 字符串处理      | sese    | V   |
| LibreSSL            | github.com/PowerShell/LibreSSL           | SSL 支持           | sese    | V   |
| sese-db             | github.com/shiinasama/sese-db            | sese 统一数据库接口     | sese    | V   |
| sqlite              | www.sqlite.org                           | SQLite 驱动        | sese-db | V   |
| mariadb-connector-c | github.com/shiinasama/mariab-connector-c | MariaDB\MySQL 驱动 | sese-db | V   |

开放的编译选项：

| 名称                  | 描述                  | 默认值 |
|---------------------|---------------------|-----|
| SESE_BUILD_TEST     | 构建 sese 的单元测试       | OFF |
| SESE_USE_SSL        | 添加 SSL 支持           | OFF |
| SESE_USE_DB         | 添加数据库接口支持           | OFF |
| SESE_DB_BUILD_TEST  | 构建 sese-db 的单元测试    | OFF |
| SESE_DB_USE_SQLITE  | 添加 SQLite 驱动        | ON  |
| SESE_DB_USE_MARIADB | 添加 MariaDB\MySQL 驱动 | OFF |

工具链的选取：

- Windows
    - MSVC v142 - VS 2019 C++ x64/x86 生成工具<br>尽管任然支持这一版本的工具链进行编译，<br>但我们还是建议您使用更高版本的
      MSVC v143 - VS 2022 C++ x64/x86 生成工具。
- Linux
    - GNU/GCC 9.50 以上，我们同样建议您使用更高的版本。
    - Clang 我们尚未进行最低版本的测试，推荐您使用 12 及其以上版本。
- macOS
    - 通常来说只要您拥有 command-line-tools 即可。

## 使用

推荐的使用方式共两种：<br>
首推使用 CMake FetchContent <br>
其次才推荐使用 git submodule

具体的使用方法请参考 [docs](docs/readme.md) 和 [example](example/CMakeLists.txt)