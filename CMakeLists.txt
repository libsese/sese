cmake_minimum_required(VERSION 3.13)

# CMP0048
# 用于允许在 Project 字段前添加 SET、OPTION 等字段
# https://cmake.org/cmake/help/latest/policy/CMP0048.html
cmake_policy(SET CMP0048 NEW)

option(SESE_USE_NATIVE_MANAGER "use the unix-like packages control" OFF)
option(SESE_BUILD_TEST "build unit test targets" OFF)
option(SESE_AUTO_VCPKG "auto find vcpkg and include toolchain" OFF)
option(SESE_DB_USE_SQLITE "add sqlite support" ON)
option(SESE_DB_USE_MARIADB "add mariadb and mysql support" OFF)
option(SESE_DB_USE_POSTGRES "add postgresql support" OFF)

include(cmake/MiniSeseTools.cmake)

sese_auto_enable_feature(SESE_BUILD_TEST "tests")
sese_auto_enable_feature(SESE_DB_USE_SQLITE "sqlite3")
sese_auto_enable_feature(SESE_DB_USE_MARIADB "mysql")
sese_auto_enable_feature(SESE_DB_USE_POSTGRES "psql")

if (WIN32 OR SESE_AUTO_VCPKG)
    sese_auto_detect_triplet()
    sese_auto_find_vcpkg()
endif ()

project(sese-core VERSION 1.3.4)
set(CMAKE_CXX_STANDARD 17)

set(SESE_REPO_HASH "UNKNOWN")
set(SESE_REPO_BRANCH "UNKNOWN")
if (EXISTS ${PROJECT_SOURCE_DIR}/.git)
    include(${CMAKE_CURRENT_LIST_DIR}/cmake/Git.cmake)
    get_git_hash(SESE_REPO_HASH ${CMAKE_CURRENT_LIST_DIR})
    get_git_branch(SESE_REPO_BRANCH ${CMAKE_CURRENT_LIST_DIR})
endif ()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebugInfo")
    add_definitions("-DSESE_IS_DEBUG")
    if (WIN32)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
        # for sanitizer
        # 0xC0000135 : https://stackoverflow.com/questions/66531482/application-crashes-when-using-address-sanitizer-with-msvc
        # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fsanitize=address /fno-sanitize-address-vcasan-lib /Zi /D_DISABLE_VECTOR_ANNOTATION /D_DISABLE_STRING_ANNOTATION")
    elseif (UNIX)
        # for valgrind
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -gdwarf-4")
        # for sanitizer
        # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
        # for coverage
        # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-update=atomic")
    endif ()
endif ()

add_definitions("-DPROJECT_PATH=\"${PROJECT_SOURCE_DIR}\"")
add_definitions("-DPROJECT_BINARY_PATH=\"${CMAKE_CURRENT_BINARY_DIR}\"")

add_subdirectory(sese)

if (SESE_BUILD_TEST)
    enable_testing()
    add_subdirectory(gtest)
    add_subdirectory(gtest-db)
    add_subdirectory(demo/echo-server)
    add_subdirectory(demo/http-client)
    add_subdirectory(demo/json2yml)
endif ()