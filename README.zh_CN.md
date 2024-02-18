<h1 align="center">Sese Library</h1>
<div align="center">
<img src="static/logo.svg" width="128" height="128" alt="logo"/>
</div>
<div align="center">
<img src="https://img.shields.io/static/v1?label=license&message=Apache-2.0&color=blue&logo=Apache" alt="license"/>
<img src="https://img.shields.io/static/v1?label=language&message=C%2B%2B%2017&color=blue&logo=cplusplus" alt="lang"/>
<img src="https://img.shields.io/static/v1?label=build%20system&message=CMake&color=blue&logo=cmake" alt="buildsystem"/>
<br>
<img alt="CodeFactor Status" src="https://www.codefactor.io/repository/github/libsese/sese-core/badge"/>
<img alt="GitHub Workflow Status (with event)" src="https://img.shields.io/github/actions/workflow/status/libsese/sese-core/windows-2022.yml?label=Windows&logo=windows">
<img alt="GitHub Workflow Status (with event)" src="https://img.shields.io/github/actions/workflow/status/libsese/sese-core/ubuntu-22.04-apt.yml?label=Ubuntu&logo=ubuntu">
<img alt="GitHub Workflow Status (with event)" src="https://img.shields.io/github/actions/workflow/status/libsese/sese-core/macos-12-brew.yml?label=macOS&logo=apple">
<br>
</div>

## SESE-CORE

SESE-CODE 是一个使用 C++17 语言标准、以 CMake 作为构建系统、以 vcpkg 作为包管理器的跨平台基础项目，可以用于系统基础开发以及
Web 开发等。

项目目前支持在 x86_64 架构下的 Windows、macOS 以及各个 Linux 发行版上运行， ARM 架构尚未进行测试。

项目定位是一定程度上作为标准库的补充使用（例如 folly、boost 等），例如异步、日志、线程、二进制接口等这些常用的功能。

## 构建

使用 vcpkg 导入依赖则不太需要你关系具体如何构建这个项目。只需要导入私有的 vcpkg-registry，填写 vcpkg.json 并添加 vcpkg
工具链即可。

如果是编译安装或者作为开发者本地开发或调试本项目同样需要使用 vcpkg。 clone 项目并配置该项目，添加 vcpkg 工具链和
SESE_BUILD_TEST=ON 以构建单元测试。

使用以下脚本自动配置当前项目至 build 目录下：

```bash
python3 scripts/config.py
```

## CODESPACES

使用 CODESPACES 只需要以项目的默认的配置创建容器即可。
.vscode/settings.json 提供了该容器可直接使用的默认配置。

## 自构建容器

项目也提供了 Dockerfile 文件构建一个可以直接用于开发的容器环境，具体细节可以参考 docker/readme.md 文件，此处不再赘述。

## 单元测试

项目选用了 googletest 作为项目的单元测试框架，部分测试屏蔽了 SIGPIPE 信号以保证正常运行。
使用 gdb 或 lldb 调试时，可能会自动加载项目下的 **.gdbinit** 或 **.lldbinit** 文件屏蔽所有的 SIGPIPE 信号。
运行所有测试只需要进入构建目录并运行 ctest 即可。

## 测试报告

生成测试报告需要编译器支持并设置 FLAGS 以生成报告数据。而生成报告则依赖于 gcovr，可以通过 pip 或者系统的包管理系统安装。
对于一些已经标记过时、弃用的实现，已经通过 **gcovr.cfg** 进行了过滤，使用以下命令可以直接在 coverage 目录下生成测试报告。

```bash
python3 scripts/make_coverage_report.py
```

有时候你可能需要通过 gcovr 的某些选项来选择特定版本的 gcov 可执行文件。

## WORKFLOWS

针对支持的三大平台均有工作流进行自动测试。其中在 ubuntu 的 test_coverage_job 中将会生成一份名为 CoverageReport.zip
的测试报告文件。

## 更多

需要更多信息，请移步 [documents](https://libsese.github.io/sese-docs/#/)。