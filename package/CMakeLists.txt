cmake_minimum_required(VERSION 3.13)

project(package_test)

add_executable(main main.cpp)

find_package(Sese CONFIG REQUIRED)
target_link_libraries(main PRIVATE Sese::Core)