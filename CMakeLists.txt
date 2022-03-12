cmake_minimum_required(VERSION 3.0)
project(sese CXX)

set(CMAKE_CXX_STANDARD 20)
include_directories(./src)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/output)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/output)
enable_testing()

set(RECORD_SRCS
        src/record/AbstractAppender.cpp
        src/record/AbstractAppender.h
        src/record/Event.h
        src/record/Formatter.h
        src/record/SimpleFormatter.cpp
        src/record/SimpleFormatter.h
        src/record/ConsoleAppender.cpp
        src/record/ConsoleAppender.h
        src/record/Logger.cpp
        src/record/Logger.h
        )

set(UTIL_SRCS
        src/util/AbstractStringBuffer.cpp
        src/util/AbstractStringBuffer.h
        src/util/Array.h
        src/util/Noncopyable.h
        src/util/Singleton.h
        src/util/StringBuilder.h
        src/util/Test.cpp
        src/util/Test.h
        src/util/DateTime.cpp
        src/util/DateTime.h
        src/util/TimeSpan.cpp
        src/util/TimeSpan.h
        src/util/Exception.h
        src/util/IndexOutOfBoundsException.h
        )

set(ROOT_SRCS
        src/Util.cpp
        src/Util.h
        )

set(ALL_SRCS
        ${UTIL_SRCS}
        ${ROOT_SRCS}
        ${RECORD_SRCS}
        )

function(build type)
    if (${type} MATCHES "shared")
        add_definitions(-DWINDOWS_DLL)
        add_library(sese SHARED ${ALL_SRCS})
    endif (${type} MATCHES "shared")
    if (${type} MATCHES "static")
        add_library(sese STATIC ${ALL_SRCS})
    endif (${type} MATCHES "static")
endfunction(build)

# build 参数可以是 shared 和 static，代表其生成类型
build(shared)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    message(STATUS "Add library : thread")
    find_package(Threads)
    target_link_libraries(sese ${CMAKE_THREAD_LIBS_INIT})
endif (CMAKE_SYSTEM_NAME MATCHES "Linux")

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    message(STATUS "Add library : wsock32")
    target_link_libraries(sese wsock32)
#    message(STATUS "Add library : dbghelp")
#    target_link_libraries(sese dbghelp)
endif (CMAKE_SYSTEM_NAME MATCHES "Windows")

function(reg_test test_name)
    add_executable(${test_name} test/${test_name}/main.cpp)
    target_link_libraries(${test_name} sese)
    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction(reg_test)

reg_test(TestArray)
reg_test(TestAbstractStringBuffer)
reg_test(TestSingleton)
reg_test(TestLogger)
reg_test(TestDebug)
reg_test(TestDateTime)