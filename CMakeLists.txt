cmake_minimum_required(VERSION 3.14)
project(sese-db)

include(FetchContent)

FetchContent_Declare(mariadb-connector-c
        GIT_REPOSITORY https://github.com/shiinasama/mariadb-connector-c.git
        GIT_TAG 6ed5f4e649a510a9a687f3667ebcc2dc5564c03e
        )
FetchContent_MakeAvailable(mariadb-connector-c)

enable_testing()

add_executable(TestMariaDriver)
target_sources(TestMariaDriver PRIVATE test/TestMariaDriver.cpp)
target_link_libraries(TestMariaDriver libmariadb)