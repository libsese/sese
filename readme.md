# Sese Library

Sese 是一个跨平台基础库，以**静态链接库**或**动态链接库**方式部署至解决方案中。

Sese 支持**多平台**编译，包括但不限于 Windows、Linux、macOS、Android，可按需要进行移植。

Sese 自<kbd>[7da500c](https://github.com/SHIINASAMA/sese/tree/7da500cfba4a7cbbc6071e686c2a6780236f7db3)</kbd>起，所有类均经过 **CTest** 与 **Valgrind**[[0]](#jmp0) 内存分析。

Sese 只支持在64位系统下运行，非64位不会进行适配，带来的效果与性能均不能保证。

 ## 构建

### Windows

使用 **MSVC v143 - VS 2022 C++ X86 生成工具** 、**CMake** 。

默认自动链接 wsock32、ws2_32 系统网络库 和 dbghelp 调试库，其中后者为非必要项，添加 **-DNEED_DBGHELP** 开关自动链接。

为消除歧义，Windows 下生成的**静态链接库**[[1]](#jmp1)将以 "*.a" 方式命名。

### Linux

已在以下环境进行测试，仅供参考。

- WSL2-Arch 使用 **Clang 13**、**CMake**。
- WSL2-Ubuntu 使用 **Clang 10**、**CMake**。
- Ubuntu 使用 **GCC 11.2.0**、**CMake**

默认自动链接 pthread 库。

### macOS

使用 **Clang 12**、**CMake**。

### Android

在 Windows 下使用 NDK + CMake 编译

NDK 版本为 24.0.8215888，仅供参考。

在使用 CMake 时需要添加额外参数

```bash
# 交叉编译必须指定 target 系统名称
-DCMAKE_SYSTEM_NAME=Android
# 设置 CMake 工具链
-DCMAKE_TOOLCHAIN_FILE="${ANDROID_HOME}/ndk/${NDK_VERSION}/build/cmake/android.toolchain.cmake"
# 设置 NDK Make 路径
-DCMAKE_MAKE_PROGRAM="${ANDROID_HOME}/ndk/${NDK_VERSION}/prebuilt/windows-x86_64/bin/make.exe"
# 设置 NDK 编译器路径
-DCMAKE_C_COMPILER="${ANDROID_HOME}/ndk/${NDK_VERSION}/toolchains/llvm/prebuilt/windows-x86_64/bin/clang.exe"
-DCMAKE_CXX_COMPILER="${ANDROID_HOME}/ndk/${NDK_VERSION}/toolchains/llvm/prebuilt/windows-x86_64/bin/clang++.exe"
# 选择生成架构
-DANDROID_ABI=arm64-v8a
# NDK 工具链选择 Clang
-DCMAKE_ANDROID_NDK_TOOLCHAIN_VERSION=clang
# Environment 相关，见下方注意事项
-DGIT_EXECUTABLE="path/to/git"
```

### 通用

** 注意 ** ：为保证 Environment 提供完整信息，还需保证以下两点

- 确保项目以**克隆**方式被保存下来。

- 如果 git 本体不在系统环境变量中时，请手动指定 **-DGIT_EXECUTABLE** 位置。

  ``` bash
  cmake -DGIT_EXECUTABLE="path/to/git"
  ```

## 其他

<span id="jmp0"/>

- Valgrind：在 macOS 无法通过 brew 直接安装，使用的是 [LouisBrunner/valgrind-macos](https://github.com/LouisBrunner/valgrind-macos) 编译安装。Windows 上没有相关实现，可以用 Visual Studio 自带的分析。

<span id="jmp1"/>

- 静态链接库：在 Windows 下生成的 静态链接库文件以 "\*.lib" 方式命名，与动态链接库生成的符号文件重名，并且会导致 Ninja 生成规则失败。故所以静态链接库均以 "\*.a"方式命名

