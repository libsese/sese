# 贡献指南

## 配置项目

~~项目配置使用 CMake + vcpkg 的形式。CMake 负责构建，vcpkg 负责管理依赖。~~

现在同样支持 unix-like 原生包管理。

对于 debian/ubuntu:

```bash
apt build-dep .
```

对于 macOS:

```bash
brew install openssl libarchive googletest
```

通常的配置步骤:

- 下载项目

  你需要选择使用 git clone 该项目而不是直接下载源代码压缩包，因为我们需要版本管理.
  ```bash
  git clone https://github.com/libsese/sese-core.git
  ```

- 配置 CMake

  注：使用原生包管理则无需配置 vcpkg 和指定 CMAKE_TOOLCHAIN_FILE。

  首先你需要确保你的机器上有配置完成的 vcpkg 工具。
  配置 CMake 有几种方案可以选择，如果你在使用 vscode，你可能需要安装 CMake 插件并在项目配置中添加选项

  <kbd>-DCMAKE_TOOLCHAIN_FILE=/usr/local/vcpkg/scripts/buildsystems/vcpkg.cmake</kbd>

  使用 visual studio 或者 clion 也和 vscode 的配置大差不差，除了该选项以外还需要添加

  <kbd>-DSESE_BUILD_TEST=ON</kbd>

  以打开构建单元测试的开关，这会自动添加 google test 的依赖项并添加项目的单元测试目标。

  完整的 CMake 构建命令看起来应该是这样的：
  ```bash
  cmake -DCMAKE_TOOLCHAIN_FILE=/usr/local/vcpkg/scripts/buildsystems/vcpkg.cmake -DSESE_BUILD_TEST=ON
  ```

- 代码规范

  风格、格式、检查等配置应当是被启用的，项目中的有 **.clang-format** / **.clang-tidy** / **.editconfig** 等。
  这对于项目的维护和新功能的开发是必要的。

## 预设的开发环境

项目在 .vscode 下预设了两个可用于 windows 和 unix-like 的 vscode 工作区文件，在依赖齐全的情况下基本做到开箱可用。

对于喜欢使用开发容器的人来说，项目同样提供了开发容器，只需在容器内启用 unix 工作区即可。

值得注意的是，在 macOS 中你可能需要重新指定工作区中 CMake 生成器的位置，因为使用 brew 安装的 ninja 通常不在 /usr/bin/ninja。使用

```bash
where ninja
```
查看 ninja 完整路径。

## 分支管理

<kbd>main</kbd> 是本项目的主分支，大多数时候不允许贡献代码在未经许可的情况下上传到这里。

需要修改代码可以选择 fork 本项目到个人账户下，并在完成开发后向上游发起 pull request；
或者你可能是组织成员，那么你有权限在本仓库新建一个开发分支，分支名称通常应该应该符合以下形式

<kbd>dev-feature</kbd>、
<kbd>dev-new-feature</kbd>、
<kbd>fix-bug</kbd> 等

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

## 联系方式

对部分代码存在疑问或者是其它问题时，你可以先尝试联系代码片段的贡献者以获取帮助。

或联系项目[原作者](https://github.com/shiinasama/)咨询与获取可提供的帮助。