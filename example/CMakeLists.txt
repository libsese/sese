cmake_minimum_required(VERSION 3.14)
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