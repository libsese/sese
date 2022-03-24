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
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/output)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/output)
enable_testing()

# 版本宏
message(STATUS "Project Version : ${CMAKE_PROJECT_VERSION}-${SESE_REPO_BRANCH}.${SESE_REPO_HASH}")
add_definitions(-DSESE_MAJOR_VERSION=${SESE_MAJOR_VERSION})
add_definitions(-DSESE_MINOR_VERSION=${SESE_MINOR_VERSION})
add_definitions(-DSESE_REPO_HASH="${SESE_REPO_HASH}")
add_definitions(-DSESE_REPO_BRANCH="${SESE_REPO_BRANCH}")

set(ALL_SRCS
        src/record/AbstractAppender.cpp
        src/record/SimpleFormatter.cpp
        src/record/ConsoleAppender.cpp
        src/record/Logger.cpp
        src/record/FileAppender.cpp
        src/AbstractStringBuffer.cpp
        src/Test.cpp
        src/DateTime.cpp
        src/TimeSpan.cpp
        src/DateTimeFormatter.cpp
        src/Random.cpp
        src/Util.cpp
        src/thread/Thread.cpp
        src/system/Environment.cpp
        src/system/CpuInfo.cpp
        )

message(STATUS "C Compiler : ${CMAKE_C_COMPILER}")
message(STATUS "CXX Compiler : ${CMAKE_CXX_COMPILER}")

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(NATIVE_WINDOWS_SRCS)
    add_definitions(-DWINDOWS_DLL)
    add_definitions(-DNEED_DBGHELP)
    add_library(sese SHARED ${ALL_SRCS} ${NATIVE_WINDOWS_SRCS})

    message(STATUS "Add library : wsock32")
    target_link_libraries(sese wsock32)
    #    message(STATUS "Add library : dbghelp")
    #    target_link_libraries(sese dbghelp)
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(NATIVE_LINUX_SRCS)
    add_library(sese SHARED ${ALL_SRCS} ${NATIVE_LINUX_SRCS})
    set_target_properties(sese PROPERTIES VERSION ${PROJECT_VERSION})

    message(STATUS "Add library : thread")
    find_package(Threads)
    target_link_libraries(sese ${CMAKE_THREAD_LIBS_INIT})
endif ()

# 安装脚本
install(TARGETS sese
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
        DESTINATION "include/${PROJECT_NAME}")

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