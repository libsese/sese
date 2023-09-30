find_package(GTest CONFIG REQUIRED)
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
        TestAddress.cpp
        TestArgParser.cpp
        TestAsyncLogger.cpp
        TestBase64.cpp
        TestBufferedStream.cpp
        TestBufferWrapper.cpp
        TestByteBuilder.cpp
        TestCompressor.cpp
        TestConfig.cpp
        TestCookies.cpp
        TestCSVUtil.cpp
        TestDateTime.cpp
        TestDNS.cpp
        TestEncodingConverter.cpp
        TestEndian.cpp
        TestEnv.cpp
        TestException.cpp
        TestFakeStream.cpp
        TestFileNotifier.cpp
        TestFileStream.cpp
        TestFixedBuffer.cpp
        TestHttp.cpp
        TestHttp2.cpp
        TestHttpClient.cpp
        TestHttpService.cpp
        TestHttpService_V2.cpp
        TestHttpUtil.cpp
        TestIni.cpp
        TestInit.cpp
        TestIOBuf.cpp
        TestIOCP.cpp
        TestJson.cpp
        TestLibraryLoader.cpp
        TestLogger.cpp
        TestMemoryViewer.cpp
        TestMessageDigest.cpp
        TestNetworkUtil.cpp
        TestOutputUtil.cpp
        TestPercentConverter.cpp
        TestPlugin/TestPlugin.cpp
        TestProcess.cpp
        TestRandom.cpp
        TestRange.cpp
        TestReusableSocket.cpp
        TestRpc.cpp
        TestService.cpp
        TestSharedMemory/TestSharedMemory.cpp
        TestSocket.cpp
        TestSSL.cpp
        TestSString.cpp
        TestStackInfo.cpp
        TestStopWatch.cpp
        TestStream.cpp
        TestStringBuilder.cpp
        TestThread.cpp
        TestTimer.cpp
        TestUniReader.cpp
        TestUtil.cpp
        TestUuid.cpp
        TestWebsocketService.cpp
        TestXML.cpp
        TestYaml.cpp
)

target_link_libraries(AllTestInMain Core GTest::gtest GTest::gtest_main)
add_test(NAME AllTestInMain COMMAND AllTestInMain)

# Test Plugin
add_library(Module SHARED TestPlugin/Module.cpp)
target_link_libraries(Module PUBLIC Core)
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