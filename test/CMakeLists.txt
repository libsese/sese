if (WIN32)
    add_compile_options(/utf-8)
endif ()

# StringBuilder
add_executable(TestStringBuilder ${CMAKE_CURRENT_LIST_DIR}/TestStringBuilder/main.cpp)
target_link_libraries(TestStringBuilder sese)
add_test(NAME TestStringBuilder COMMAND TestStringBuilder)

# Singleton
add_executable(TestSingleton ${CMAKE_CURRENT_LIST_DIR}/TestSingleton/main.cpp)
target_link_libraries(TestSingleton sese)
add_test(NAME TestSingleton COMMAND TestSingleton)

# Logger
add_executable(TestLogger ${CMAKE_CURRENT_LIST_DIR}/TestLogger/main.cpp)
target_link_libraries(TestLogger sese)
add_test(NAME TestLogger COMMAND TestLogger)

# DateTime
add_executable(TestDateTime ${CMAKE_CURRENT_LIST_DIR}/TestDateTime/main.cpp)
target_link_libraries(TestDateTime sese)
add_test(NAME TestDateTime COMMAND TestDateTime)

# DateTimeFormatter
add_executable(TestDateTimeFormatter ${CMAKE_CURRENT_LIST_DIR}/TestDateTimeFormatter/main.cpp)
target_link_libraries(TestDateTimeFormatter sese)
add_test(NAME TestDateTimeFormatter COMMAND TestDateTimeFormatter)

# Thread
add_executable(TestThread ${CMAKE_CURRENT_LIST_DIR}/TestThread/main.cpp)
target_link_libraries(TestThread sese)
add_test(NAME TestThread COMMAND TestThread)

# Environment
add_executable(TestEnvironment ${CMAKE_CURRENT_LIST_DIR}/TestEnvironment/main.cpp)
target_link_libraries(TestEnvironment sese)
add_test(NAME TestEnvironment COMMAND TestEnvironment)

# Random
add_executable(TestRandom ${CMAKE_CURRENT_LIST_DIR}/TestRandom/main.cpp)
target_link_libraries(TestRandom sese)
add_test(NAME TestRandom COMMAND TestRandom)

# ArgParser
add_executable(TestArgParser ${CMAKE_CURRENT_LIST_DIR}/TestArgParser/main.cpp)
target_link_libraries(TestArgParser sese)
add_test(NAME TestArgParser0 COMMAND TestArgParser a1=hello a2="a sentence" a3=123)
add_test(NAME TestArgParser1 COMMAND TestArgParser a1=hello a3=123)

# ByteBuilder
add_executable(TestByteBuilder ${CMAKE_CURRENT_LIST_DIR}/TestByteBuilder/main.cpp)
target_link_libraries(TestByteBuilder sese)
add_test(NAME TestByteBuilder COMMAND TestByteBuilder)

# Endian
add_executable(TestEndian ${CMAKE_CURRENT_LIST_DIR}/TestEndian/main.cpp)
target_link_libraries(TestEndian sese)
add_test(NAME TestEndian COMMAND TestEndian)

# Address
add_executable(TestAddress ${CMAKE_CURRENT_LIST_DIR}/TestAddress/main.cpp)
target_link_libraries(TestAddress sese)
add_test(NAME TestAddress COMMAND TestAddress)

# Locker
add_executable(TestLocker ${CMAKE_CURRENT_LIST_DIR}/TestLocker/main.cpp)
target_link_libraries(TestLocker sese)
add_test(NAME TestLocker COMMAND TestLocker)

# ThreadPool
add_executable(TestThreadPool ${CMAKE_CURRENT_LIST_DIR}/TestThreadPool/main.cpp)
target_link_libraries(TestThreadPool sese)
add_test(NAME TestThreadPool COMMAND TestThreadPool)

# Socket
add_executable(TestSocket ${CMAKE_CURRENT_LIST_DIR}/TestSocket/main.cpp)
target_link_libraries(TestSocket sese)
add_test(NAME TestSocket COMMAND TestSocket)

# StreamReader
add_executable(TestStreamReader ${CMAKE_CURRENT_LIST_DIR}/TestStreamReader/main.cpp)
target_link_libraries(TestStreamReader sese)
add_test(NAME TestStreamReader COMMAND TestStreamReader)

# UniReader
add_executable(TestUniReader ${CMAKE_CURRENT_LIST_DIR}/TestUniReader/main.cpp)
target_link_libraries(TestUniReader sese)
add_test(NAME TestUniReader COMMAND TestUniReader)

# ConfigUtil
add_executable(TestConfigUtil ${CMAKE_CURRENT_LIST_DIR}/TestConfigUtil/main.cpp)
target_link_libraries(TestConfigUtil sese)
add_test(NAME TestConfigUtil COMMAND TestConfigUtil)

# Base64Converter
add_executable(TestBase64Converter ${CMAKE_CURRENT_LIST_DIR}/TestBase64Converter/main.cpp)
target_link_libraries(TestBase64Converter sese)
add_test(NAME TestBase64Converter COMMAND TestBase64Converter)

# PercentConverter
add_executable(TestPercentConverter ${CMAKE_CURRENT_LIST_DIR}/TestPercentConverter/main.cpp)
target_link_libraries(TestPercentConverter sese)
add_test(NAME TestPercentConverter COMMAND TestPercentConverter)

# LibraryLoader
add_executable(TestLibraryLoader ${CMAKE_CURRENT_LIST_DIR}/TestLibraryLoader/main.cpp)
target_link_libraries(TestLibraryLoader sese)
add_test(NAME TestLibraryLoader COMMAND TestLibraryLoader)

# MD5Util
add_executable(TestMD5Util ${CMAKE_CURRENT_LIST_DIR}/TestMD5Util/main.cpp)
target_link_libraries(TestMD5Util sese)
add_test(NAME TestMD5Util COMMAND TestMD5Util)

# Debug
add_executable(TestDebug ${CMAKE_CURRENT_LIST_DIR}/TestDebug/main.cpp)
target_link_libraries(TestDebug sese)
add_test(NAME TestDebug COMMAND TestDebug)

# Initializer
add_executable(TestInitializer ${CMAKE_CURRENT_LIST_DIR}/TestInitializer/main.cpp)
target_link_libraries(TestInitializer sese)
add_test(NAME TestInitializer COMMAND TestInitializer)

# TcpServer
add_executable(TestTcpServer ${CMAKE_CURRENT_LIST_DIR}/TestTcpServer/main.cpp)
target_link_libraries(TestTcpServer sese)
add_test(NAME TestTcpServer COMMAND TestTcpServer)

# Timer
add_executable(TestTimer ${CMAKE_CURRENT_LIST_DIR}/TestTimer/main.cpp)
target_link_libraries(TestTimer sese)
add_test(NAME TestTimer COMMAND TestTimer)

# JsonUtil
add_executable(TestJsonUtil ${CMAKE_CURRENT_LIST_DIR}/TestJsonUtil/main.cpp)
target_link_libraries(TestJsonUtil sese)
add_test(NAME TestJsonUtil COMMAND TestJsonUtil)

# Http
add_executable(TestHttp ${CMAKE_CURRENT_LIST_DIR}/TestHttp/main.cpp)
target_link_libraries(TestHttp sese)
add_test(NAME TestHttp COMMAND TestHttp)

# ObjectPool
add_executable(TestObjectPool ${CMAKE_CURRENT_LIST_DIR}/TestObjectPool/main.cpp)
target_link_libraries(TestObjectPool sese)
add_test(NAME TestObjectPool COMMAND TestObjectPool)

# BufferedStream
add_executable(TestBufferedStream ${CMAKE_CURRENT_LIST_DIR}/TestBufferedStream/main.cpp)
target_link_libraries(TestBufferedStream sese)
add_test(NAME TestBufferedStream COMMAND TestBufferedStream)

# Concurrent
add_executable(TestConcurrent ${CMAKE_CURRENT_LIST_DIR}/TestConcurrent/main.cpp)
target_link_libraries(TestConcurrent sese)
add_test(NAME TestConcurrent COMMAND TestConcurrent)

# UrlHelper
add_executable(TestUrlHelper ${CMAKE_CURRENT_LIST_DIR}/TestUrlHelper/main.cpp)
target_link_libraries(TestUrlHelper sese)
add_test(NAME TestUrlHelper COMMAND TestUrlHelper)

# Httpd
add_executable(TestHttpd ${CMAKE_CURRENT_LIST_DIR}/TestHttpd/main.cpp)
target_link_libraries(TestHttpd sese)
add_test(NAME TestHttpd COMMAND TestHttpd)

# AddressPool
add_executable(TestAddressPool ${CMAKE_CURRENT_LIST_DIR}/TestAddressPool/main.cpp)
target_link_libraries(TestAddressPool sese)
add_test(NAME TestAddressPool COMMAND TestAddressPool)

# HttpClient
add_executable(TestHttpClient ${CMAKE_CURRENT_LIST_DIR}/TestHttpClient/main.cpp)
target_link_libraries(TestHttpClient sese)
add_test(NAME TestHttpClient COMMAND TestHttpClient)

# BlockAppender
add_executable(TestBlockAppender ${CMAKE_CURRENT_LIST_DIR}/TestBlockAppender/main.cpp)
target_link_libraries(TestBlockAppender sese)
add_test(NAME TestBlockAppender COMMAND TestBlockAppender)

# TextReader
add_executable(TestTextReader ${CMAKE_CURRENT_LIST_DIR}/TestTextReader/main.cpp)
target_link_libraries(TestTextReader sese)
add_test(NAME TestTextReader COMMAND TestTextReader)

# XmlUtil
add_executable(TestXmlUtil ${CMAKE_CURRENT_LIST_DIR}/TestXmlUtil/main.cpp)
target_link_libraries(TestXmlUtil sese)
add_test(NAME TestXmlUtil COMMAND TestXmlUtil)

# Rpc
add_executable(TestRpc ${CMAKE_CURRENT_LIST_DIR}/TestRpc/main.cpp)
target_link_libraries(TestRpc sese)
add_test(NAME TestRpc COMMAND TestRpc)

# SHA256Util
add_executable(TestSHA256Util ${CMAKE_CURRENT_LIST_DIR}/TestSHA256Util/main.cpp)
target_link_libraries(TestSHA256Util sese)
add_test(NAME TestSHA256Util COMMAND TestSHA256Util)

# SharedMemory
add_executable(TestSharedMemory ${CMAKE_CURRENT_LIST_DIR}/TestSharedMemory/main.cpp)
target_link_libraries(TestSharedMemory sese)
add_test(NAME TestSharedMemory COMMAND TestSharedMemory)

# BufferWrapper
add_executable(TestBufferWrapper ${CMAKE_CURRENT_LIST_DIR}/TestBufferWrapper/main.cpp)
target_link_libraries(TestBufferWrapper sese)
add_test(NAME TestBufferWrapper COMMAND TestBufferWrapper)

if (SESE_USE_SSL)
    # SSL
    add_executable(TestSSL ${CMAKE_CURRENT_LIST_DIR}/TestSSL/main.cpp)
    target_link_libraries(TestSSL sese)
    add_test(NAME TestSSL COMMAND TestSSL)
endif ()

# Cookie
add_executable(TestCookie ${CMAKE_CURRENT_LIST_DIR}/TestCookie/main.cpp)
target_link_libraries(TestCookie sese)

# UUID
add_executable(TestUuid ${CMAKE_CURRENT_LIST_DIR}/TestUuid/main.cpp)
target_link_libraries(TestUuid sese)
add_test(NAME TestUuid COMMAND TestUuid)

# SHA1Util
add_executable(TestSHA1Util ${CMAKE_CURRENT_LIST_DIR}/TestSHA1Util/main.cpp)
target_link_libraries(TestSHA1Util sese)
add_test(NAME TestSHA1Util COMMAND TestSHA1Util)