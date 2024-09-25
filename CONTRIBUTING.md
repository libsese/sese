# 贡献指南

## 配置项目

项目配置使用 CMake + vcpkg 的形式。CMake 负责构建，vcpkg 负责管理依赖。

现在也同样部分支持 unix-like 原生包管理。

对于 debian/ubuntu:

```bash
./scripts/install_ubuntu_deps.sh
```

对于 macOS:

```bash
./scripts/install_drawin_deps.sh
```

通常的配置步骤:

- 下载项目

  你需要选择使用 git clone 该项目而不是直接下载源代码压缩包，因为我们需要版本管理.
  ```bash
  git clone https://github.com/libsese/sese.git
  ```

- 配置 CMake

  注：使用原生包管理则无需配置 vcpkg 和指定 CMAKE_TOOLCHAIN_FILE，只需要设置 SESE_USE_NATIVE_MANAGER=ON 即可。

  首先你需要确保你的机器上有配置完成的 vcpkg 工具。
  配置 CMake 有几种方案可以选择，如果你在使用 vscode，你可能需要安装 CMake 插件并在项目配置中添加选项

  `-DCMAKE_TOOLCHAIN_FILE=/usr/local/vcpkg/scripts/buildsystems/vcpkg.cmake`

  使用 visual studio 或者 clion 也和 vscode 的配置大差不差，除了该选项以外还需要添加

  `-DSESE_BUILD_TEST=ON`

  以打开构建单元测试的开关，这会自动添加 google test 的依赖项并添加项目的单元测试目标。

  完整的 CMake 构建命令看起来应该是这样的：
  ```bash
  cmake -DCMAKE_TOOLCHAIN_FILE=/usr/local/vcpkg/scripts/buildsystems/vcpkg.cmake -DSESE_BUILD_TEST=ON
  ```

  如果你不希望使用 vcpkg 的清单模式，你可以将 `VCPKG_MANIFEST_MODE` 设置为 OFF 以强制使用经典模式。

- 代码规范

  风格、格式、检查等配置应当是被启用的，项目中的有 **.clang-format** / **.clang-tidy** / **.editconfig** 等。
  这对于项目的维护和新功能的开发是必要的。

## 预设的开发环境

预设配置现在应该参考 **CMakePresets.json**，如果你使用的 CMake 或 IDE 版本过低，你可能需要手动配置这些开发选项。

对于喜欢使用开发容器的人来说，项目同样提供了开发容器，~~只需在容器内启用 unix 工作区即可~~。

值得注意的是，在 macOS 中你可能需要重新指定工作区中 CMake 生成器的位置，因为使用 brew 安装的 ninja 通常不在 /usr/bin/ninja。使用

```bash
where ninja
```
查看 ninja 完整路径。

## 关于 ARM 架构

> [!WARNING]
> ARM 目前并不属于主要适配的架构，在一定程度上项目能在 ARM 上运行。
> 如果你执意使用相关设备进行开发和运行，产生的效果我们概不负责，当然也欢迎有相关方面经验的开发者贡献兼容性代码。
> 下面的内容将**简单的**、**不严谨的**为在 x86_64/amd64 的开发者提供一种可行的 ARM64 模拟方案。

为 docker 添加跨架构支持，这**可能**需要你手动安装 `qemu` 的相关组件，此处不进行演示。详细信息请参考 [tonistiigi/binfmt](https://github.com/tonistiigi/binfmt)。

```bash
docker run --privileged --rm tonistiigi/binfmt --install arm64
```

[ubuntu-arm64.dockerfile](./docker/ubuntu-arm64.dockerfile) 提供了基于 ARM64 Ubuntu 的基础运行环境（不包括数据库软件），可以使用 docker 直接构建镜像。

```bash
docker build -f docker/ubuntu-arm64.dockerfile .
```

使用此镜像可以完整构建并运行相关测试用例。

## 分支管理

`main` 是本项目的主分支，大多数时候不允许贡献代码在未经许可的情况下上传到这里。

需要修改代码可以选择 fork 本项目到个人账户下，并在完成开发后向上游发起 pull request；
或者你可能是组织成员，那么你有权限在本仓库新建一个开发分支，分支名称通常应该应该符合以下形式

`dev-[username]`、
`dev-feature`、
`dev-new-feature`、
`fix-bug` 等

## 提交流程

在完成开发后，如果你的本地或个人仓库中的 commit 过于复杂繁琐，那么你应该先将 commit 进行适当压缩再向上游发起 pr。

当你完成新功能或者是修复等工作时，你应该确保项目中所有的单元测试是能够正常通过的；尤其是在 github 的 workflow
中，网络、系统等环境和本机不一定一致有可能会导致单元测试能在本机通过但将在 workflow 中失败。

确认无误后你可以向上游发起 pull request，你需要描述清楚你对现有代码的更改很新增代码的功能。你也可以选择向项目已有的贡献者或者工作人员发送电子邮件以详细描述你所完成的工作。

提交信息格式可以归纳如下

> { commit type }: { title }
>
> { detail(option) }

其中 commit type 通常有以下几种选择:

- feat - 新功能
- fix - 修复
- perf - 性能改进
- refactor - 重构
- build - 构建变更
- chore - 杂项
- docs - 文档
- style - 代码风格变更
- test - 测试用例变更

## 问题和需求

在编写具体代码前，你应该分析你要解决的问题，明确的你需求是什么，然后分析现有的代码是否已经包含类似的功能。

对于新的功能你需要确保这符合项目所提供的功能的职责范围。对于旧的实现变更，你需要确保这些变更不会对已有的实现产生较大的不兼容。
不得不对一些底层代码进行一些破坏性变更时，你应该重新实现一个版本，通过 **namespace** + **using/typedef** 等方式保留旧的、添加新的实现。

通常一次更改应该是原子的、局部的，也就是说每次你应该只修改一个功能、遇到实现 A 不得不改 B 的时候，你应该先联系相关人员征求意见，测试对 B 的更改不会影响 B 原有的使用。

项目通常不允许直接为其添加新的依赖，引入新的依赖需要经过详细研究和充分讨论决定。

## 贡献类型

贡献的类型大致可以参考 [提交流程](#提交流程)

## 测试要求

项目使用 googletest 进行测试，测试并不要求 100% 的覆盖率，而是应该针对最常用的功能添加最全面的覆盖，作用相对不大的功能可以酌情不测试一些非极端场景。

对于一些可能难以测试的函数或分支，最好是添加详细的注释表明此处为何没有做到覆盖，可能存在的风险，以及出现该情况的应对方案。

## 发行流程

此部分规范适用于项目核心管理人员，需要严格安装下列操作进行

1. 变更项目版本号，修改 `CMakeLists.txt` 和 `vcpkg.json`，提交将会自动触发 `update_portfile.py`，等待 CI 测试结果。
2. 打上 tag 等待 CI 自动发版。

## 联系方式

对部分代码存在疑问或者是其它问题时，你可以先尝试联系代码片段的贡献者以获取帮助。

或联系项目[原作者](https://github.com/shiinasama/)咨询与获取可提供的帮助。