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

add_library(sese-http
        src/HttpServiceImpl.h
        src/HttpServiceImpl.cpp
        src/HttpConnection.cpp
        src/HttpConnectionImpl.cpp
        src/HttpConnectionEx.cpp
        src/HttpConnectionExImpl.cpp
)
target_link_libraries(sese-http PRIVATE asio::asio Core)

add_definitions("-DPROJECT_PATH=\"${PROJECT_SOURCE_DIR}\"")

add_executable(NewHttp1Impl
        src/test/NewHttp1Impl.cpp
)
target_link_libraries(NewHttp1Impl PRIVATE asio::asio Core sese-http)
