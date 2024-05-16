cmake_minimum_required(VERSION 3.13)

set(VCPKG_OVERLAY_PORTS "${CMAKE_CURRENT_LIST_DIR}/ports")

project(package_test)

add_executable(main main.cpp)

find_package(Sese CONFIG REQUIRED)
target_link_libraries(main PRIVATE Sese::Core)