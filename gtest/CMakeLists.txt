find_package(GTest CONFIG REQUIRED)
find_package(sese-plugin CONFIG REQUIRED)
find_package(Python3 COMPONENTS Interpreter REQUIRED)

if (WIN32)
    add_compile_options(/utf-8)
endif ()

macro(sese_test test_name)
    add_executable(${test_name} ${test_name}.cpp)
    target_link_libraries(${test_name} Core GTest::gtest GTest::gtest_main)
    add_test(NAME ${test_name} COMMAND ${test_name})
endmacro()

sese_test(TestAddress)
sese_test(TestArgParser)
sese_test(TestBase64)
sese_test(TestLogger)
sese_test(TestEndian)
sese_test(TestJson)
sese_test(TestStream)
sese_test(TestConfig)
sese_test(TestDateTime)
sese_test(TestThread)
sese_test(TestMessageDigest)
sese_test(TestCompressor)
sese_test(TestProcess)

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
add_dependencies(TestPlugin Module)

sese_test(TestCSVUtil)

sese_test(TestHttp2)
target_compile_definitions(TestHttp2 PRIVATE "-DPY_EXECUTABLE=\"${Python3_EXECUTABLE}\"")

sese_test(TestNetworkUtil)
sese_test(TestServer)
sese_test(TestHttp)
sese_test(TestRandom)