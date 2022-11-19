<div align=center>
 <h1><i>Sese Library</i></h1>
</div>
<div align=center>
 <img src="https://img.shields.io/static/v1?label=license&message=Apache-2.0&color=red"/>
 <img src="https://img.shields.io/static/v1?label&message=linux&color=blue&logo=linux"/>
 <img src="https://img.shields.io/static/v1?label&message=windows&color=blue&logo=windows"/>
 <img src="https://img.shields.io/static/v1?label&message=macOS&color=blue&logo=macos"/>
 <img src="https://img.shields.io/static/v1?label=language&message=C%2B%2B20&color=greed&logo=cplusplus"/>
 <img src="https://img.shields.io/static/v1?label=build%20system&message=CMake&color=greed&logo=cmake"/>
</div>
<div align=center>
 <a href="mailto://shiina_kaoru@outlook.com">
  <img src="https://img.shields.io/static/v1?label=email&message=SHIINA_KAORU@Outlook.com&color=skyblue&logo=gmail"/>
 </a>
 <img src="https://img.shields.io/static/v1?label=QQ&message=995602964&color=skyblue&logo=tencentqq"/>
</div>
<div align=center>
    <i>When the belly is full,the mind is among the maids. Sese will not requesting anythings.</i>
</div>
<br>

Sese 是一个跨平台基础库，以**静态链接库**或**动态链接库**方式部署至解决方案中。

Sese 支持**多平台**编译，包括但不限于 Windows、Linux、macOS，可按需要进行移植。

Sese 自<kbd>[7da500c](https://github.com/SHIINASAMA/sese/tree/7da500cfba4a7cbbc6071e686c2a6780236f7db3)</kbd>起，所有类均经过 **CTest** 与 **Valgrind**[[0]](#jmp0) 内存分析。

Sese 只支持在64位系统下运行，非64位不会进行适配，带来的效果与性能均不能保证。

 ## 构建

### Windows

使用 **MSVC v143 - VS 2022 C++ AMD64 生成工具** 、**CMake** 。

默认自动链接 ws2_32 系统网络库 和 dbghelp 调试库，其中后者为非必要项，添加 **-DNEED_DBGHELP** 开关自动链接。

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

- 使用 [git submodule](docs/readme.md#git%20submodule)

- 使用 [CMake FetchContent (New)](docs/readme.md#CMake%20FetchContent)

## 其他

- 示例程序以及一些必要细节请参阅 [Sese's Docs](docs/readme.md) 。

<span id="jmp0"/>

- Valgrind：在 macOS 无法通过 brew 直接安装，使用的是 [LouisBrunner/valgrind-macos](https://github.com/LouisBrunner/valgrind-macos) 编译安装。Windows 上没有相关实现，可以用 Visual Studio 自带的分析。

