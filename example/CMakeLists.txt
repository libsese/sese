cmake_minimum_required(VERSION 3.14)
project(example)

set(CMAKE_CXX_STANDARD 20)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/target)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/target)

include(FetchContent)
FetchContent_Declare(sese
        GIT_REPOSITORY https://github.com/SHIINASAMA/sese.git
        GIT_TAG ${HASH_VALUE_OR_TAG_NAME}
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/lib/sese
        )
FetchContent_GetProperties(sese)
if(NOT sese_POPULATED)
    FetchContent_MakeAvailable(sese)
endif()

add_executable(example src/main.cpp)
target_link_libraries(example sese)