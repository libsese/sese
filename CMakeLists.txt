cmake_minimum_required(VERSION 3.13)

include(sese/cmake/MiniSeseTools.cmake)

project(sese-http)
set(CMAKE_CXX_STANDARD 20)
set(SESE_DB_USE_SQLITE OFF)
set(SESE_BUILD_TEST ON)

add_subdirectory(sese)

if (MSVC)
    add_compile_options(/utf-8)
    add_definitions(-D_WIN32_WINNT=0x0601)
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

add_executable(NewHttpImpl
        src/test/NewHttpImpl.cpp
)
target_link_libraries(NewHttpImpl PRIVATE asio::asio Core sese-http)
