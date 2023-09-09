find_package(GTest CONFIG REQUIRED)
find_package(sese-plugin CONFIG REQUIRED)
find_package(Python3 COMPONENTS Interpreter REQUIRED)

set(CMAKE_CXX_STANDARD 20)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/utf-8)
endif ()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")
    set(CMAKE_EXE_LINKER_FLAGS "-Wl,-copy-dt-needed-entries,--export-dynamic")
endif ()

add_definitions("-DPY_EXECUTABLE=\"${Python3_EXECUTABLE}\"")

add_executable(
        AllTestInMain
        AllTest.cpp
        TestLogger.cpp
        TestAsyncLogger.cpp
        TestPlugin/TestPlugin.cpp
        TestSharedMemory/TestSharedMemory.cpp
        TestAddress.cpp
        TestArgParser.cpp
        TestBase64.cpp
        TestEndian.cpp
        TestJson.cpp
        TestXML.cpp
        TestIni.cpp
        TestYaml.cpp
        TestStream.cpp
        TestConfig.cpp
        TestDateTime.cpp
        TestThread.cpp
        TestMessageDigest.cpp
        TestCompressor.cpp
        TestProcess.cpp
        TestCSVUtil.cpp
        TestHttp2.cpp
        TestNetworkUtil.cpp
        # TestServer.cpp
        TestHttp.cpp
        TestRandom.cpp
        TestFileNotifier.cpp
        TestReusableSocket.cpp
        TestService.cpp
        TestEnv.cpp
        TestInit.cpp
        TestLibraryLoader.cpp
        TestTimer.cpp
        TestByteBuilder.cpp
        TestFixedBuffer.cpp
        TestOutputUtil.cpp
        TestBufferWrapper.cpp
        TestStringBuilder.cpp
        TestBufferedStream.cpp
        TestFileStream.cpp
        TestMemoryViewer.cpp
        TestUtil.cpp
        TestUuidBuilder.cpp
        TestFakeStream.cpp
        TestEncodingConverter.cpp
        TestPercentConverter.cpp
        TestSSL.cpp
        TestUniReader.cpp
        TestSocket.cpp
        TestHttpUtil.cpp
        TestHttpClient.cpp
        TestRpc.cpp
        TestHttpService.cpp
        TestHttpService_V2.cpp
        TestWebsocketService.cpp
        TestDNS.cpp
        TestRange.cpp
        TestStackInfo.cpp
)

target_link_libraries(AllTestInMain Core GTest::gtest GTest::gtest_main)
add_test(NAME AllTestInMain COMMAND AllTestInMain)

# Test Plugin
add_library(Module SHARED TestPlugin/Module.cpp)
target_link_libraries(Module PUBLIC Sese::Plugin)
set_target_properties(
        Module PROPERTIES
        OUTPUT_NAME "Module"
        PREFIX ""
        SUFFIX ".m"
)
target_compile_definitions(AllTestInMain PRIVATE "-DPATH_TO_MODULE=\"$<TARGET_FILE:Module>\"")
target_compile_definitions(AllTestInMain PRIVATE "-DPATH_TO_CORE=\"$<TARGET_FILE:Core>\"")
add_dependencies(AllTestInMain Module)

# TestSharedMemory
add_executable(Memory.d TestSharedMemory/Memory.d.cpp)
target_link_libraries(Memory.d Core)
set_target_properties(
        Memory.d PROPERTIES
        OUTPUT_NAME "Memory.d"
        PREFIX ""
)
target_compile_definitions(AllTestInMain PRIVATE "-DPATH_TO_MEM_D=\"$<TARGET_FILE:Memory.d>\"")
add_dependencies(AllTestInMain Memory.d)