find_package(GTest CONFIG REQUIRED)
find_package(sese-plugin CONFIG REQUIRED)
find_package(Python3 COMPONENTS Interpreter REQUIRED)

set(CMAKE_CXX_STANDARD 20)

if (WIN32)
    add_compile_options(/utf-8)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")
    set(CMAKE_EXE_LINKER_FLAGS "-Wl,-copy-dt-needed-entries")
endif ()

add_definitions("-DPY_EXECUTABLE=\"${Python3_EXECUTABLE}\"")

macro(sese_test test_name)
    add_executable(${test_name} ${test_name}.cpp)
    target_link_libraries(${test_name} Core GTest::gtest GTest::gtest_main)
    add_test(NAME ${test_name} COMMAND ${test_name})
endmacro()

# Test Plugin
add_library(Module SHARED TestPlugin/Module.cpp)
target_link_libraries(Module PUBLIC Sese::Plugin)
set_target_properties(
        Module PROPERTIES
        OUTPUT_NAME "Module"
        PREFIX ""
        SUFFIX ".m"
)
add_executable(TestPlugin TestPlugin/TestPlugin.cpp)
target_link_libraries(TestPlugin Core GTest::gtest GTest::gtest_main)
add_test(NAME TestPlugin COMMAND $<TARGET_FILE:TestPlugin>)
target_compile_definitions(TestPlugin PRIVATE "-DPATH_TO_MODULE=\"$<TARGET_FILE:Module>\"")
target_compile_definitions(TestPlugin PRIVATE "-DPATH_TO_CORE=\"$<TARGET_FILE:Core>\"")
add_dependencies(TestPlugin Module)

# TestSharedMemory
add_executable(Memory.d TestSharedMemory/Memory.d.cpp)
target_link_libraries(Memory.d Core)
set_target_properties(
        Memory.d PROPERTIES
        OUTPUT_NAME "Memory.d"
        PREFIX ""
)
add_executable(TestSharedMemory TestSharedMemory/TestSharedMemory.cpp)
target_link_libraries(TestSharedMemory Core GTest::gtest GTest::gtest_main)
target_compile_definitions(TestSharedMemory PRIVATE "-DPATH_TO_MEM_D=\"$<TARGET_FILE:Memory.d>\"")
add_test(NAME TestSharedMemory COMMAND TestSharedMemory)
add_dependencies(TestSharedMemory Memory.d)

sese_test(TestAddress)
sese_test(TestArgParser)
sese_test(TestBase64)
sese_test(TestLogger)
sese_test(TestEndian)
sese_test(TestJson)
sese_test(TestXML)
sese_test(TestIni)
sese_test(TestYaml)
sese_test(TestStream)
sese_test(TestConfig)
sese_test(TestDateTime)
sese_test(TestThread)
sese_test(TestMessageDigest)
sese_test(TestCompressor)
sese_test(TestProcess)
sese_test(TestCSVUtil)
sese_test(TestHttp2)
sese_test(TestNetworkUtil)
# sese_test(TestServer)
sese_test(TestHttp)
sese_test(TestRandom)
sese_test(TestFileNotifier)
sese_test(TestReusableSocket)
sese_test(TestService)
sese_test(TestEnv)
sese_test(TestInit)
sese_test(TestLibraryLoader)
sese_test(TestTimer)
sese_test(TestByteBuilder)
sese_test(TestFixedBuffer)
sese_test(TestOutputUtil)
sese_test(TestBufferWrapper)
sese_test(TestStringBuilder)
sese_test(TestBufferedStream)
sese_test(TestFileStream)
sese_test(TestMemoryViewer)
sese_test(TestUtil)
sese_test(TestUuidBuilder)
sese_test(TestAsyncLogger)
sese_test(TestFakeStream)
sese_test(TestEncodingConverter)
sese_test(TestPercentConverter)