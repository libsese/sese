# Array
add_executable(TestArray test/TestArray/main.cpp)
target_link_libraries(TestArray SeseSharedLibrary)
add_test(NAME TestArray COMMAND TestArray)

# StringBuilder
add_executable(TestStringBuilder test/TestStringBuilder/main.cpp)
target_link_libraries(TestStringBuilder SeseSharedLibrary)
add_test(NAME TestStringBuilder COMMAND TestStringBuilder)

# Singleton
add_executable(TestSingleton test/TestSingleton/main.cpp)
target_link_libraries(TestSingleton SeseSharedLibrary)
add_test(NAME TestSingleton COMMAND TestSingleton)

# Logger
add_executable(TestLogger test/TestLogger/main.cpp)
target_link_libraries(TestLogger SeseSharedLibrary)
add_test(NAME TestLogger COMMAND TestLogger)

# Debug
add_executable(TestDebug test/TestDebug/main.cpp)
target_link_libraries(TestDebug SeseSharedLibrary)
add_test(NAME TestDebug COMMAND TestDebug)

# DateTime
add_executable(TestDateTime test/TestDateTime/main.cpp)
target_link_libraries(TestDateTime SeseSharedLibrary)
add_test(NAME TestDateTime COMMAND TestDateTime)

# DateTimeFormatter
add_executable(TestDateTimeFormatter test/TestDateTimeFormatter/main.cpp)
target_link_libraries(TestDateTimeFormatter SeseSharedLibrary)
add_test(NAME TestDateTimeFormatter COMMAND TestDateTimeFormatter)

# Thread
add_executable(TestThread test/TestThread/main.cpp)
target_link_libraries(TestThread SeseSharedLibrary)
add_test(NAME TestThread COMMAND TestThread)

# Environment
add_executable(TestEnvironment test/TestEnvironment/main.cpp)
target_link_libraries(TestEnvironment SeseSharedLibrary)
add_test(NAME TestEnvironment COMMAND TestEnvironment)

# CpuInfo
add_executable(TestCpuInfo test/TestCpuInfo/main.cpp)
target_link_libraries(TestCpuInfo SeseSharedLibrary)
add_test(NAME TestCpuInfo COMMAND TestCpuInfo)

# Random
add_executable(TestRandom test/TestRandom/main.cpp)
target_link_libraries(TestRandom SeseSharedLibrary)
add_test(NAME TestRandom COMMAND TestRandom)

# ArgParser
add_executable(TestArgParser test/TestArgParser/main.cpp)
target_link_libraries(TestArgParser SeseSharedLibrary)
add_test(NAME TestArgParser0 COMMAND TestArgParser a1=hello a2="a sentence" a3=123)
add_test(NAME TestArgParser1 COMMAND TestArgParser a1=hello a3=123)

# ByteBuilder
add_executable(TestByteBuilder test/TestByteBuilder/main.cpp)
target_link_libraries(TestByteBuilder SeseSharedLibrary)
add_test(NAME TestByteBuilder COMMAND TestByteBuilder)