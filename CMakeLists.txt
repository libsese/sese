cmake_minimum_required(VERSION 3.13)

# CMP0048
# 用于允许在 Project 字段前添加 SET、OPTION 等字段
# https://cmake.org/cmake/help/latest/policy/CMP0048.html
cmake_policy(SET CMP0048 NEW)

option(SESE_USE_NATIVE_MANAGER "use the unix-like packages control" OFF)
option(SESE_BUILD_TEST "build unit test targets" OFF)
option(SESE_BUILD_EXAMPLE "build example targets" OFF)
option(SESE_AUTO_VCPKG "auto find vcpkg and include toolchain" OFF)
option(SESE_USE_ASYNC_LOGGER "use async logger" OFF)
option(SESE_USE_ARCHIVE "add archive support" OFF)
option(SESE_REPLACE_EXECINFO "replace the system execinfo implementation" OFF)
option(SESE_DB_USE_SQLITE "add sqlite support" ON)
option(SESE_DB_USE_MARIADB "add mariadb and mysql support" OFF)
option(SESE_DB_USE_POSTGRES "add postgresql support" OFF)

if (SESE_USE_NATIVE_MANAGER)
    MESSAGE(FATAL_ERROR "The `SESE_USE_NATIVE_MANAGER` option has been deprecated.")
endif ()

include(cmake/MiniSeseTools.cmake)

sese_auto_enable_feature(SESE_BUILD_TEST "tests")
sese_auto_enable_feature(SESE_USE_ARCHIVE "archive")
sese_auto_enable_feature(SESE_REPLACE_EXECINFO "replace-execinfo")
sese_auto_enable_feature(SESE_DB_USE_SQLITE "sqlite3")
sese_auto_enable_feature(SESE_DB_USE_MARIADB "mysql")
sese_auto_enable_feature(SESE_DB_USE_POSTGRES "psql")

# if (WIN32 OR SESE_AUTO_VCPKG)
#     sese_auto_find_vcpkg()
# endif ()

project(sese VERSION 2.3.0)
# for cxx 20 modules
# set(CMAKE_CXX_EXTENSIONS OFF)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SESE_REPO_HASH "UNKNOWN")
set(SESE_REPO_BRANCH "UNKNOWN")
if (EXISTS ${PROJECT_SOURCE_DIR}/.git)
    include(${CMAKE_CURRENT_LIST_DIR}/cmake/Git.cmake)
    get_git_hash(SESE_REPO_HASH ${CMAKE_CURRENT_LIST_DIR})
    get_git_branch(SESE_REPO_BRANCH ${CMAKE_CURRENT_LIST_DIR})
endif ()

if (NOT "${CMAKE_BUILD_TYPE}" MATCHES "Debug")
    if (MSVC)
        set(${CMAKE_CXX_FLAGS} "${CMAKE_CXX_FLAGS} /Ox")
    else ()
        set(${CMAKE_CXX_FLAGS} "${CMAKE_CXX_FLAGS} -O3")
    endif ()
endif ()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebugInfo")
    add_definitions("-DSESE_IS_DEBUG")
    if (MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
        # for sanitizer
        # 0xC0000135 : https://stackoverflow.com/questions/66531482/application-crashes-when-using-address-sanitizer-with-msvc
        # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fsanitize=address /fno-sanitize-address-vcasan-lib /D_DISABLE_VECTOR_ANNOTATION /D_DISABLE_STRING_ANNOTATION")
    else ()
        # unittest coverage
        # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-update=atomic")
    endif ()

    if (UNIX)
        # for valgrind
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -gdwarf-4")
        # for sanitizer
        # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    endif ()
endif ()

add_subdirectory(sese)
