# 项目信息
cmake_minimum_required(VERSION 3.12)
set(SESE_MAJOR_VERSION 0)
set(SESE_MINOR_VERSION 1)
project(sese VERSION ${SESE_MAJOR_VERSION}.${SESE_MINOR_VERSION})

# Git 版本信息
set(SESE_REPO_HASH "")
set(SESE_REPO_BRANCH "")
include(Git.cmake)
get_git_hash(SESE_REPO_HASH)
get_git_branch(SESE_REPO_BRANCH)

# 项目详情
set(CMAKE_CXX_STANDARD 20)
include_directories(${PROJECT_SOURCE_DIR}/include/)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/output/${CMAKE_SYSTEM_NAME})
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/output/${CMAKE_SYSTEM_NAME})
enable_testing()

# 版本宏
message(STATUS "Project Version : ${CMAKE_PROJECT_VERSION}-${SESE_REPO_BRANCH}.${SESE_REPO_HASH}")
add_definitions(-DSESE_MAJOR_VERSION=${SESE_MAJOR_VERSION})
add_definitions(-DSESE_MINOR_VERSION=${SESE_MINOR_VERSION})
add_definitions(-DSESE_REPO_HASH="${SESE_REPO_HASH}")
add_definitions(-DSESE_REPO_BRANCH="${SESE_REPO_BRANCH}")

set(ALL_SRCS
        # 日志
        src/record/AbstractAppender.cpp
        src/record/SimpleFormatter.cpp
        src/record/ConsoleAppender.cpp
        src/record/Logger.cpp
        src/record/FileAppender.cpp
        # 根目录
        src/AbstractStringBuffer.cpp
        src/ArgParser.cpp
        src/AbstractByteBuffer.cpp
        src/DateTime.cpp
        src/TimeSpan.cpp
        src/DateTimeFormatter.cpp
        src/StringBuffer.cpp
        src/ByteBuffer.cpp
        # src/Random.cpp
        src/Util.cpp
        # 线程
        src/thread/Thread.cpp
        # 系统
        src/system/Environment.cpp
        # 网络
        src/net/Address.cpp
        src/net/IPAddress.cpp
        src/net/IPv4Address.cpp
        src/net/IPv6Address.cpp
        )

set(NATIVE_PATH "src/native/${CMAKE_SYSTEM_NAME}")
message(STATUS "C Compiler : ${CMAKE_C_COMPILER}")
message(STATUS "CXX Compiler : ${CMAKE_CXX_COMPILER}")
message(STATUS "NATIVE PATH : ${NATIVE_PATH}")

set(NATIVE_SRCS
        ${NATIVE_PATH}/Test.cpp
        ${NATIVE_PATH}/system/CpuInfo.cpp
        )

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_definitions(-DWINDOWS_DLL)
    add_definitions(-DNEED_DBGHELP)

    add_library(SeseSharedLibrary SHARED ${ALL_SRCS} ${NATIVE_SRCS})
    add_library(SeseStaticLibrary STATIC ${ALL_SRCS} ${NATIVE_SRCS})

    message(STATUS "Add library : wsock32, ws2_32")
    target_link_libraries(SeseSharedLibrary wsock32 ws2_32)
    target_link_libraries(SeseStaticLibrary wsock32 ws2_32)
    #    message(STATUS "Add library : dbghelp")
    #    target_link_libraries(sese dbghelp)
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_compile_options(-fPIC)
    add_library(SeseSharedLibrary SHARED ${ALL_SRCS} ${NATIVE_SRCS})
    add_library(SeseStaticLibrary STATIC ${ALL_SRCS} ${NATIVE_SRCS})

    message(STATUS "Add library : thread")
    find_package(Threads)
    target_link_libraries(SeseSharedLibrary ${CMAKE_THREAD_LIBS_INIT})
    target_link_libraries(SeseStaticLibrary ${CMAKE_THREAD_LIBS_INIT})
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    add_library(SeseSharedLibrary SHARED ${ALL_SRCS} ${NATIVE_SRCS})
    add_library(SeseStaticLibrary STATIC ${ALL_SRCS} ${NATIVE_SRCS})
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Android")
    add_library(SeseSharedLibrary SHARED ${ALL_SRCS} ${NATIVE_SRCS})
    add_library(SeseStaticLibrary STATIC ${ALL_SRCS} ${NATIVE_SRCS})
endif ()

set_target_properties(
        SeseSharedLibrary
        PROPERTIES
        OUTPUT_NAME sese
        VERSION ${PROJECT_VERSION}
)

set_target_properties(
        SeseStaticLibrary
        PROPERTIES
        OUTPUT_NAME sese
        VERSION ${PROJECT_VERSION}
        SUFFIX ".a"
)

include(Test.cmake)

# 安装脚本
install(TARGETS SeseSharedLibrary SeseStaticLibrary
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
        DESTINATION "include/${PROJECT_NAME}")