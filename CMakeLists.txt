cmake_minimum_required(VERSION 3.12)
project(sese VERSION 0.4.1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(SESE_REPO_HASH)
set(SESE_REPO_BRANCH)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/Git.cmake)
get_git_hash(SESE_REPO_HASH ${CMAKE_CURRENT_LIST_DIR})
get_git_branch(SESE_REPO_BRANCH ${CMAKE_CURRENT_LIST_DIR})

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/target)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/target)

option(SESE_BUILD_TEST "build unit test targets" OFF)
option(SESE_USE_SSL "import ssl dependencies" ON)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/Dep.cmake)

message(STATUS "==================== SESE ====================")
message(STATUS "Project version: ${PROJECT_VERSION}")
message(STATUS "C STANDARD VERSION: ${CMAKE_C_STANDARD}")
message(STATUS "CXX STANDARD VERSION: ${CMAKE_CXX_STANDARD}")
message(STATUS "Repository branch: ${SESE_REPO_BRANCH}")
message(STATUS "Repository hash: ${SESE_REPO_HASH}")
message(STATUS "C COMPILER PATH: ${CMAKE_C_COMPILER}")
message(STATUS "CXX COMPILER PATH: ${CMAKE_CXX_COMPILER}")

add_definitions("-DSESE_MAJOR_VERSION=\"${PROJECT_VERSION_MAJOR}\"")
add_definitions("-DSESE_MINOR_VERSION=\"${PROJECT_VERSION_MINOR}\"")
add_definitions("-DSESE_PATCH_VERSION=\"${PROJECT_VERSION_PATCH}\"")
add_definitions("-DSESE_REPO_HASH=\"${SESE_REPO_HASH}\"")
add_definitions("-DSESE_REPO_BRANCH=\"${SESE_REPO_BRANCH}\"")
add_definitions("-DPROJECT_PATH=\"${CMAKE_CURRENT_LIST_DIR}\"")

add_library(sese SHARED)
add_library(sese-static STATIC)

target_include_directories(sese PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_include_directories(sese-static PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

target_sources(sese PRIVATE
        src/record/AbstractAppender.cpp src/record/SimpleFormatter.cpp
        src/record/ConsoleAppender.cpp src/record/Logger.cpp
        src/record/LogHelper.cpp src/record/FileAppender.cpp
        src/record/BlockAppender.cpp

        src/convert/EncodingConverter.cpp src/convert/Base64Converter.cpp
        src/convert/PercentConverter.cpp src/convert/MD5Util.cpp
        src/convert/SHA256Util.cpp

        src/thread/Thread.cpp src/thread/ThreadPool.cpp

        src/system/Environment.cpp src/system/LibraryLoader.cpp

        src/net/Address.cpp src/net/IPAddress.cpp
        src/net/IPv4Address.cpp src/net/IPv6Address.cpp
        src/net/AddressPool.cpp
        src/net/http/Header.cpp src/net/http/QueryString.cpp
        src/net/http/HttpUtil.cpp src/net/http/UrlHelper.cpp
        src/net/http/HttpClient.cpp src/net/http/HttpServer.cpp
        src/net/rpc/Server.cpp src/net/rpc/Client.cpp

        src/config/ConfigUtil.cpp
        src/config/json/JsonTypes.cpp src/config/json/JsonUtil.cpp
        src/config/xml/XmlTypes.cpp src/config/xml/XmlUtil.cpp

        src/text/DateTimeFormatter.cpp src/text/TextReader.cpp

        src/AbstractStringBuffer.cpp src/ArgParser.cpp
        src/AbstractByteBuffer.cpp src/BufferedStream.cpp
        src/BufferedOutputStream.cpp src/BufferedInputStream.cpp
        src/DateTime.cpp src/TimeSpan.cpp
        src/StringBuffer.cpp src/ByteBuffer.cpp
        src/Random.cpp src/Util.cpp
        src/FileStream.cpp src/Test.cpp
        src/Timer.cpp src/Initializer.cpp
        src/MemoryViewer.cpp src/ConsoleOutputStream.cpp
        src/OutputBufferWrapper.cpp src/InputBufferWrapper.cpp
        )

if (SESE_USE_SSL)
    target_compile_definitions(sese PRIVATE -DSESE_USE_SSL)
    target_compile_definitions(sese-static PRIVATE -DSESE_USE_SSL)
    target_sources(sese PRIVATE
            src/security/SecurityConfig.cpp src/security/SSLContext.cpp
            src/security/SSLContextBuilder.cpp src/security/SecuritySocket.cpp
            )
    target_link_libraries(sese crypto ssl)
    target_link_libraries(sese-static crypto ssl)
endif ()

set(NATIVE_PATH "src/native/${CMAKE_SYSTEM_NAME}")
target_sources(sese PRIVATE
        ${NATIVE_PATH}/Test.cpp
        ${NATIVE_PATH}/net/Socket.cpp
        ${NATIVE_PATH}/net/TcpServer.cpp
        ${NATIVE_PATH}/config/UniReader.cpp
        ${NATIVE_PATH}/system/SharedMemory.cpp
        )

target_sources(sese-static PRIVATE $<TARGET_OBJECTS:sese>)

if (WIN32)
    set(DLL_INFO_RC ${PROJECT_BINARY_DIR}/WindowsDllInfo.rc)
    configure_file(${CMAKE_CURRENT_LIST_DIR}/WindowsDllInfo.rc.in ${DLL_INFO_RC})

    set_target_properties(sese sese-static PROPERTIES
            DEBUG_POSTFIX d
            )
    target_sources(sese PRIVATE ${DLL_INFO_RC})
    target_compile_definitions(sese PUBLIC -DWINDOWS_DLL)
    target_compile_definitions(sese PUBLIC -DNEED_DBGHELP)
    target_compile_options(sese PRIVATE "/utf-8")
    target_compile_options(sese PRIVATE "/wd4251")
    target_compile_options(sese PRIVATE "/wd4530")
    target_link_libraries(sese ws2_32)

    target_compile_definitions(sese-static PUBLIC -DNEED_DBGHELP)
    target_compile_options(sese-static PRIVATE "/utf-8")
    target_compile_options(sese-static PRIVATE "/wd4251")
    target_compile_options(sese-static PRIVATE "/wd4530")
    target_link_libraries(sese-static ws2_32)

    message(STATUS "Linked libraries: ws2_32, dbghelp")
endif ()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_package(Threads REQUIRED)

    target_compile_options(sese PUBLIC "-fPIC")
    target_link_libraries(sese ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})
    target_link_libraries(sese-static ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})

    message(STATUS "Linked libraries: pthread, dl")
endif ()

target_link_libraries(sese SString-static)
target_link_libraries(sese-static SString-static crypto-static ssl-static)

message(STATUS "C compile option: ${CMAKE_C_FLAGS}")
message(STATUS "CXX compile options: ${CMAKE_CXX_FLAGS}")

if (SESE_BUILD_TEST)
    enable_testing()
    add_subdirectory(test)
endif ()