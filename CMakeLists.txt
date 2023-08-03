cmake_minimum_required(VERSION 3.10)

option(SESE_BUILD_TEST "build unit test targets" OFF)
if (SESE_BUILD_TEST)
    if (NOT DEFINED VCPKG_MANIFEST_FEATURES)
        message(STATUS "Auto append features: tests")
        set(VCPKG_MANIFEST_FEATURES "tests")
    else ()
        list(FIND VCPKG_MANIFEST_FEATURES "tests" index)
        if (index EQUAL -1)
            message(STATUS "Auto append features: tests")
            list(APPEND VCPKG_MANIFEST_FEATURES "tests")
        endif ()
    endif ()
endif ()

project(sese-core VERSION 0.7.6)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SESE_REPO_HASH "UNKNOWN")
set(SESE_REPO_BRANCH "UNKNOWN")
if (EXISTS ${PROJECT_SOURCE_DIR}/.git)
    include(${CMAKE_CURRENT_LIST_DIR}/cmake/Git.cmake)
    get_git_hash(SESE_REPO_HASH ${CMAKE_CURRENT_LIST_DIR})
    get_git_branch(SESE_REPO_BRANCH ${CMAKE_CURRENT_LIST_DIR})
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (WIN32)
        # for sanitizer
        # 0xC0000135 : https://stackoverflow.com/questions/66531482/application-crashes-when-using-address-sanitizer-with-msvc
        # set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /fsanitize=address /fno-sanitize-address-vcasan-lib /Zi /D_DISABLE_VECTOR_ANNOTATION /D_DISABLE_STRING_ANNOTATION")
    elseif (UNIX)
        # for valgrind
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -gdwarf-4")
        # for sanitizer
        # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
        # for coverage
        # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-update=atomic")
        # disable exceptions
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
    endif ()
endif ()

message(STATUS "==================== SESE-CORE ====================")
message(STATUS "Project version: ${PROJECT_VERSION}")
message(STATUS "C STANDARD VERSION: ${CMAKE_C_STANDARD}")
message(STATUS "CXX STANDARD VERSION: ${CMAKE_CXX_STANDARD}")
message(STATUS "Repository branch: ${SESE_REPO_BRANCH}")
message(STATUS "Repository hash: ${SESE_REPO_HASH}")
message(STATUS "C COMPILER PATH: ${CMAKE_C_COMPILER}")
message(STATUS "CXX COMPILER PATH: ${CMAKE_CXX_COMPILER}")

add_definitions("-DSESE_MAJOR_VERSION=\"${PROJECT_VERSION_MAJOR}\"")
add_definitions("-DSESE_MINOR_VERSION=\"${PROJECT_VERSION_MINOR}\"")
add_definitions("-DSESE_PATCH_VERSION=\"${PROJECT_VERSION_PATCH}\"")
add_definitions("-DSESE_REPO_HASH=\"${SESE_REPO_HASH}\"")
add_definitions("-DSESE_REPO_BRANCH=\"${SESE_REPO_BRANCH}\"")
add_definitions("-DPROJECT_PATH=\"${CMAKE_CURRENT_LIST_DIR}\"")
add_definitions("-DPROJECT_BINARY_PATH=\"${CMAKE_CURRENT_BINARY_DIR}\"")

# message(STATUS "C compile option: ${CMAKE_C_FLAGS}")
message(STATUS "CXX compile options: ${CMAKE_CXX_FLAGS}")

add_subdirectory(sese)

if (SESE_BUILD_TEST)
    enable_testing()
    # add_subdirectory(test)
    add_subdirectory(gtest)
endif ()