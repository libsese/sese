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
include_directories(./src)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/output)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/output)
enable_testing()

message(STATUS "Project Version : ${CMAKE_PROJECT_VERSION}-${SESE_REPO_BRANCH}.${SESE_REPO_HASH}")
add_definitions(-DSESE_MAJOR_VERSION=${SESE_MAJOR_VERSION})
add_definitions(-DSESE_MINOR_VERSION=${SESE_MINOR_VERSION})
add_definitions(-DSESE_REPO_HASH="${SESE_REPO_HASH}")
add_definitions(-DSESE_REPO_BRANCH="${SESE_REPO_BRANCH}")

set(RECORD_SRCS
        src/record/AbstractAppender.cpp
        src/record/AbstractAppender.h
        src/record/Event.h
        src/record/Formatter.h
        src/record/SimpleFormatter.cpp
        src/record/SimpleFormatter.h
        src/record/ConsoleAppender.cpp
        src/record/ConsoleAppender.h
        src/record/Logger.cpp
        src/record/Logger.h
        src/record/FileAppender.cpp
        src/record/FileAppender.h
        )

set(ROOT_SRCS
        src/AbstractStringBuffer.cpp
        src/AbstractStringBuffer.h
        src/Array.h
        src/Noncopyable.h
        src/Singleton.h
        src/StringBuilder.h
        src/Test.cpp
        src/Test.h
        src/DateTime.cpp
        src/DateTime.h
        src/TimeSpan.cpp
        src/TimeSpan.h
        src/IndexOutOfBoundsException.h
        src/DateTimeFormatter.cpp
        src/DateTimeFormatter.h
        src/Exception.h
        src/LogicException.h
        src/NotInstantiable.h
        src/Random.h
        src/Random.cpp
        src/Config.h
        src/Util.cpp
        src/Util.h
        )

set(THREAD_SRCS
        src/thread/Thread.h
        src/thread/Thread.cpp
        )

set(SYSTEM_SRCS
        src/system/Environment.h
        src/system/Environment.cpp
        src/system/CpuInfo.h
        src/system/CpuInfo.cpp
        )

set(ALL_SRCS
        ${ROOT_SRCS}
        ${RECORD_SRCS}
        ${THREAD_SRCS}
        ${SYSTEM_SRCS}
        )

function(build type)
    if (${type} MATCHES "shared")
        add_definitions(-DWINDOWS_DLL)
        add_library(sese SHARED ${ALL_SRCS})
    endif (${type} MATCHES "shared")
    if (${type} MATCHES "static")
        add_library(sese STATIC ${ALL_SRCS})
    endif (${type} MATCHES "static")
endfunction(build)

# build 参数可以是 shared 和 static，代表其生成类型
build(shared)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    message(STATUS "Add library : thread")
    find_package(Threads)
    target_link_libraries(sese ${CMAKE_THREAD_LIBS_INIT})
endif (CMAKE_SYSTEM_NAME MATCHES "Linux")

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_definitions(-Dpid_t=uint32_t)
    message(STATUS "Add library : wsock32")
    target_link_libraries(sese wsock32)
    #    message(STATUS "Add library : dbghelp")
    #    target_link_libraries(sese dbghelp)
endif (CMAKE_SYSTEM_NAME MATCHES "Windows")

function(reg_test test_name)
    add_executable(${test_name} test/${test_name}/main.cpp)
    target_link_libraries(${test_name} sese)
    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction(reg_test)

reg_test(TestArray)
reg_test(TestStringBuilder)
reg_test(TestSingleton)
reg_test(TestLogger)
reg_test(TestDebug)
reg_test(TestDateTime)
reg_test(TestDateTimeFormatter)
reg_test(TestThread)
reg_test(TestEnvironment)
reg_test(TestCpuInfo)
reg_test(TestRandom)