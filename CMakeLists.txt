cmake_minimum_required(VERSION 3.21)

include(cmake/Options.cmake)
project(
    sese
    VERSION 3.0.0
    LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 20)
set(SESE_ROOT_PROJECT ON)
include(cmake/Dependencies.cmake)
include(cmake/Git.cmake)
include(cmake/Coverage.cmake)
include(cmake/Asan.cmake)

add_subdirectory(sese)
