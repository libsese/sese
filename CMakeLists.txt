cmake_minimum_required(VERSION 3.0)
project(sese CXX)

set(CMAKE_CXX_STANDARD 20)
include_directories(./src)
enable_testing()

set(UTIL_SRCS
        src/util/AbstractStringBuffer.cpp
        )

add_library(sese ${UTIL_SRCS})

function(reg_test test_name)
    add_executable(${test_name} test/${test_name}/main.cpp)
    target_link_libraries(${test_name} sese)
    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction(reg_test)

reg_test(TestHeapArray)
reg_test(TestStackArray)
reg_test(TestAbstractStringBuffer)