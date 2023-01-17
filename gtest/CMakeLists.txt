if (WIN32)
    add_compile_options(/utf-8)
endif ()

add_executable(TestAddress TestAddress.cpp)
target_link_libraries(TestAddress sese gtest_main)

add_executable(TestArgParser TestArgParser.cpp)
target_link_libraries(TestArgParser sese gtest_main)

add_executable(TestBase64 TestBase64.cpp)
target_link_libraries(TestBase64 sese gtest_main)

add_executable(TestLogger TestLogger.cpp)
target_link_libraries(TestLogger sese gtest_main)

add_executable(TestEndian TestEndian.cpp)
target_link_libraries(TestEndian sese gtest_main)

add_executable(TestJson TestJson.cpp)
target_link_libraries(TestJson sese gtest_main)