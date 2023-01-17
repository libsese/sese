if (WIN32)
    add_compile_options(/utf-8)
endif ()

add_executable(TestAddress TestAddress.cpp)
target_link_libraries(TestAddress sese gtest_main)

add_executable(TestArgParser TestArgParser.cpp)
target_link_libraries(TestArgParser sese gtest_main)

add_executable(TestBase64 TestBase64.cpp)
target_link_libraries(TestBase64 sese gtest_main)