cmake_minimum_required(VERSION 3.13)

set(VCPKG_OVERLAY_PORTS "${CMAKE_CURRENT_LIST_DIR}/ports")

project(package_test)

set(CMAKE_CXX_STANDARD 17)

if (MSVC AND VCPKG_TARGET_TRIPLET STREQUAL "x64-windows-static")
    set(CMAKE_CXX_FLAGS_DEBUG /MTd)
    set(CMAKE_CXX_FLAGS_MINSIZEREL /MT)
    set(CMAKE_CXX_FLAGS_RELEASE /MT)
    set(CMAKE_CXX_FLAGS_RELWITHDEBUGINFO /MT)
endif ()

add_executable(main main.cpp)

find_package(Sese CONFIG REQUIRED)
target_link_libraries(main PRIVATE Sese::Core)