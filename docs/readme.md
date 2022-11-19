# Docs

## Usage - 使用

### git submodule

在项目中将 sese 设置为 submodule （非必须，也可以自己手动添加）

```bash
git submodule add https://github.com/SHIINASAMA/sese.git lib/sese
```

- 配置 CMakeLists.txt，将 sese 作为子模块添加

  ```cmake
  # CMakeLists.txt
  cmake_minimum_required(VERSION 3.12)
  project(example)
  
  set(CMAKE_CXX_STANDARD 20)
  
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  
  add_subdirectory(lib/sese)
  include_directories(lib/sese/include)
  
  add_executable(example src/main.cpp)
  target_link_libraries(example sese)
  ```

- 直接构建即可，示例程序以及完整的项目树如下：

  ```cpp
  // src/main.cpp
  
  #include "sese/record/LogHelper.h"
  
  sese::LogHelper helper("example"); // NOLINT
  
  int main() {
      helper.info("This message sent by sese::LogHelper!");
      return 0;
  }
  
  // tree example -L 2
  example
  ├── CMakeLists.txt
  ├── lib
  │   └── sese
  └── src
      └── main.cpp
  ```

### CMake FetchContent

在 CMakeLists.txt 添加依赖

```cmake
# CMakeLists.txt

cmake_minimum_required(VERSION 3.23)
project(example)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(sese
        GIT_REPOSITORY https://github.com/SHIINASAMA/sese.git
        GIT_TAG ${HASH_VALUE_OR_TAG_NAME}
        )
FetchContent_MakeAvailable(sese)

add_executable(example src/main.cpp)
target_link_libraries(example sese)
```

使用 CMake 3.14+ 直接加载即自动拉取依赖。

示例代码如下：

```cpp
// src/main.cpp

#include "sese/record/LogHelper.h"

sese::LogHelper helper("example"); // NOLINT

int main() {
    helper.info("This message sent by sese::LogHelper!");
    return 0;
}

// tree example -L 2
example
├── CMakeLists.txt
└── src
    └── main.cpp
```

## Other - 其他

- 索引
  - [文件流的打开模式 - FileStream](FileStream.md)
  - [跨平台 UNICODE 字符读取 - UniReader](UniReader.md)
  - [日志输出方式 - LogHelper 和 Marco](Logger.md)
  - [套接字选项 - Socket::Option](Socket.md)
  - [日志客制化格式输出 - SimpleFormatter](SimpleFormatter.md)
  - [时间格式化输出 - DateTimeFormatter](DateTimeFormatter.md)
  - [保序初始化 - Initializer](Initializer.md)
  - [低精度计时器 - Timer](Timer.md)
  - [Xml 工具 - XmlUtil](XmlUtil.md)
  - [RPC 服务 - rpc::Server & rpc::Client](RPC.md)
