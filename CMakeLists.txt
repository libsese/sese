cmake_minimum_required(VERSION 3.13)

include(sese/cmake/MiniSeseTools.cmake)

project(sese-http)
set(CMAKE_CXX_STANDARD 20)
set(SESE_DB_USE_SQLITE OFF)

add_subdirectory(sese)

if (MSVC)
    add_compile_options(/utf-8)
endif ()

find_package(asio CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

add_library(sese-http
        src/HttpServiceImpl.h
        src/HttpServiceImpl.cpp
        src/HttpConnectionImpl.cpp
        src/HttpsConnectionImpl.cpp
)
target_link_libraries(sese-http PRIVATE asio::asio Core)

enable_testing()
add_executable(AllTestsInMain src/test/Main.cpp)
target_link_libraries(AllTestsInMain PRIVATE GTest::gtest Core sese-http)
