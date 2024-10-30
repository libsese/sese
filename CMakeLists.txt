cmake_minimum_required(VERSION 3.21)

include(cmake/Options.cmake)
project(sese
        VERSION 2.3.0
        LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 20)
include(cmake/Dependencies.cmake)
include(cmake/Git.cmake)
include(cmake/Coverage.cmake)
include(cmake/Asan.cmake)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebugInfo")
    add_definitions("-DSESE_IS_DEBUG")
    if (MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
        # for sanitizer
        # 0xC0000135 : https://stackoverflow.com/questions/66531482/application-crashes-when-using-address-sanitizer-with-msvc
        # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fsanitize=address /fno-sanitize-address-vcasan-lib /D_DISABLE_VECTOR_ANNOTATION /D_DISABLE_STRING_ANNOTATION")
    endif ()
endif ()

add_subdirectory(sese)
