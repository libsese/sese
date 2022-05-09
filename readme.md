# Sese Library

Sese 是一个跨平台基础库，以**静态链接库**或**动态链接库**方式部署至解决方案中。

Sese 支持**多平台**编译，包括但不限于 Windows、Linux、macOS，可按需要进行移植。

Sese 自<kbd>[7da500c](https://github.com/SHIINASAMA/sese/tree/7da500cfba4a7cbbc6071e686c2a6780236f7db3)</kbd>起，所有类均经过 **CTest** 与 **Valgrind**[[0]](#jmp0) 内存分析。

Sese 只支持在64位系统下运行，非64位不会进行适配，带来的效果与性能均不能保证。

 ## 构建

### Windows

使用 **MSVC v143 - VS 2022 C++ X86 生成工具** 、**CMake** 。

默认自动链接 wsock32、ws2_32 系统网络库 和 dbghelp 调试库，其中后者为非必要项，添加 **-DNEED_DBGHELP** 开关自动链接。

为消除歧义，Windows 下生成的**静态链接库**[[1]](#jmp1)将以 "*.a" 方式命名。

### Linux

已在以下环境进行测试，仅供参考。

- WSL2-ArchLinux 使用 **Clang 13**、**CMake**。
- WSL2-Ubuntu 使用 **Clang 10**、**CMake**。
- WSL2-AlmaLinux 使用 **Clang 12**、**CMake**。
- Ubuntu 使用 **GCC 11.2.0**、**CMake**。

默认自动链接 pthread、dl 库。

### macOS

使用 **Clang 12**、**CMake**。

** 注意 ** ：为保证 Environment 提供完整信息，还需保证以下两点

- 确保项目以**克隆**方式被保存下来。

- 如果 git 本体不在系统环境变量中时，请手动指定 **-DGIT_EXECUTABLE** 位置。

  ``` bash
  cmake -DGIT_EXECUTABLE="/path/to/git"
  ```

## 使用

### Windows

- CMake 编译安装

  ```bash
  cmake --build /path/to/build --target install
  ```

  \*\* 注意 \*\*：也许你需要手动指定 CMAKE_PREFIX_PATH 路径，默认值为 C:\Program Files (x86)，建议手动指定。

- Windows 传统 Cpp 配置

  1. 使用 CMake 编译所需的库。

  2. 在新项目设置中手动指定链接库与包含目录。

### 类 Unix

- CMake 编译安装

  1. CMake 直接编译安装后新建项目。

  2. 新项目配置模板。

     ```cmake
     cmake_minimum_required(VERSION 3.12)
     project(${PROJECT_NAME})
     
     find_package(sese 0.1 REQUIRED)
     include_directories(${SESE_INCLUDE_DIR})
     
     add_executable(Main main.cpp)
     target_link_libraries(Main sese)
     ```

  \*\* 注意 \*\*：库默认安装路径为 "/usr/local/lib"，可能需要添加至 **LD_LIBRARY_PATH** 中。

  ```bash
  # 永久生效请写入配置文件 $Profile("/etc/bashrc"|"~/.bashrc")
  export LD_LIBRARY_PATH="/usr/local/lib":$LD_LIBRARY_PATH
  # 刷新配置文件
  source $Profile
  ```

## 其他

- 示例程序以及一些必要细节请参阅 [Sese's Docs](docs/ReadMe.md) 。

<span id="jmp0"/>

- Valgrind：在 macOS 无法通过 brew 直接安装，使用的是 [LouisBrunner/valgrind-macos](https://github.com/LouisBrunner/valgrind-macos) 编译安装。Windows 上没有相关实现，可以用 Visual Studio 自带的分析。

<span id="jmp1"/>

- 静态链接库：在 Windows 下生成的 静态链接库文件以 "\*.lib" 方式命名，与动态链接库生成的符号文件重名，并且会导致 Ninja 生成规则失败。故所以静态链接库均以 "\*.a"方式命名

