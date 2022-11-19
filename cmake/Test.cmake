# Array
add_executable(TestArray ../test/TestArray/main.cpp)
target_link_libraries(TestArray sese)
add_test(NAME TestArray COMMAND TestArray)

# StringBuilder
add_executable(TestStringBuilder ../test/TestStringBuilder/main.cpp)
target_link_libraries(TestStringBuilder sese)
add_test(NAME TestStringBuilder COMMAND TestStringBuilder)

# Singleton
add_executable(TestSingleton ../test/TestSingleton/main.cpp)
target_link_libraries(TestSingleton sese)
add_test(NAME TestSingleton COMMAND TestSingleton)

# Logger
add_executable(TestLogger ../test/TestLogger/main.cpp)
target_link_libraries(TestLogger sese)
add_test(NAME TestLogger COMMAND TestLogger)

# DateTime
add_executable(TestDateTime ../test/TestDateTime/main.cpp)
target_link_libraries(TestDateTime sese)
add_test(NAME TestDateTime COMMAND TestDateTime)

# DateTimeFormatter
add_executable(TestDateTimeFormatter ../test/TestDateTimeFormatter/main.cpp)
target_link_libraries(TestDateTimeFormatter sese)
add_test(NAME TestDateTimeFormatter COMMAND TestDateTimeFormatter)

# Thread
add_executable(TestThread ../test/TestThread/main.cpp)
target_link_libraries(TestThread sese)
add_test(NAME TestThread COMMAND TestThread)

# Environment
add_executable(TestEnvironment ../test/TestEnvironment/main.cpp)
target_link_libraries(TestEnvironment sese)
add_test(NAME TestEnvironment COMMAND TestEnvironment)

# Random
add_executable(TestRandom ../test/TestRandom/main.cpp)
target_link_libraries(TestRandom sese)
add_test(NAME TestRandom COMMAND TestRandom)

# ArgParser
add_executable(TestArgParser ../test/TestArgParser/main.cpp)
target_link_libraries(TestArgParser sese)
add_test(NAME TestArgParser0 COMMAND TestArgParser a1=hello a2="a sentence" a3=123)
add_test(NAME TestArgParser1 COMMAND TestArgParser a1=hello a3=123)

# ByteBuilder
add_executable(TestByteBuilder ../test/TestByteBuilder/main.cpp)
target_link_libraries(TestByteBuilder sese)
add_test(NAME TestByteBuilder COMMAND TestByteBuilder)

# Endian
add_executable(TestEndian ../test/TestEndian/main.cpp)
target_link_libraries(TestEndian sese)
add_test(NAME TestEndian COMMAND TestEndian)

# Address
add_executable(TestAddress ../test/TestAddress/main.cpp)
target_link_libraries(TestAddress sese)
add_test(NAME TestAddress COMMAND TestAddress)

# Locker
add_executable(TestLocker ../test/TestLocker/main.cpp)
target_link_libraries(TestLocker sese)
add_test(NAME TestLocker COMMAND TestLocker)

# ThreadPool
add_executable(TestThreadPool ../test/TestThreadPool/main.cpp)
target_link_libraries(TestThreadPool sese)
add_test(NAME TestThreadPool COMMAND TestThreadPool)

# Socket
add_executable(TestSocket ../test/TestSocket/main.cpp)
target_link_libraries(TestSocket sese)
add_test(NAME TestSocket COMMAND TestSocket)

# StreamReader
add_executable(TestStreamReader ../test/TestStreamReader/main.cpp)
target_link_libraries(TestStreamReader sese)
add_test(NAME TestStreamReader COMMAND TestStreamReader)

# UniReader
add_executable(TestUniReader ../test/TestUniReader/main.cpp)
target_link_libraries(TestUniReader sese)
add_test(NAME TestUniReader COMMAND TestUniReader)

# ConfigUtil
add_executable(TestConfigUtil ../test/TestConfigUtil/main.cpp)
target_link_libraries(TestConfigUtil sese)
add_test(NAME TestConfigUtil COMMAND TestConfigUtil)

# Base64Converter
add_executable(TestBase64Converter ../test/TestBase64Converter/main.cpp)
target_link_libraries(TestBase64Converter sese)
add_test(NAME TestBase64Converter COMMAND TestBase64Converter)

# PercentConverter
add_executable(TestPercentConverter ../test/TestPercentConverter/main.cpp)
target_link_libraries(TestPercentConverter sese)
add_test(NAME TestPercentConverter COMMAND TestPercentConverter)

# LibraryLoader
add_executable(TestLibraryLoader ../test/TestLibraryLoader/main.cpp)
target_link_libraries(TestLibraryLoader sese)
add_test(NAME TestLibraryLoader COMMAND TestLibraryLoader)

# MD5Util
add_executable(TestMD5Util ../test/TestMD5Util/main.cpp)
target_link_libraries(TestMD5Util sese)
add_test(NAME TestMD5Util COMMAND TestMD5Util)

# Debug
add_executable(TestDebug ../test/TestDebug/main.cpp)
target_link_libraries(TestDebug sese)
add_test(NAME TestDebug COMMAND TestDebug)

# Initializer
add_executable(TestInitializer ../test/TestInitializer/main.cpp)
target_link_libraries(TestInitializer sese)
add_test(NAME TestInitializer COMMAND TestInitializer)

# TcpServer
add_executable(TestTcpServer ../test/TestTcpServer/main.cpp)
target_link_libraries(TestTcpServer sese)
add_test(NAME TestTcpServer COMMAND TestTcpServer)

# Timer
add_executable(TestTimer ../test/TestTimer/main.cpp)
target_link_libraries(TestTimer sese)
add_test(NAME TestTimer COMMAND TestTimer)

# JsonUtil
add_executable(TestJsonUtil ../test/TestJsonUtil/main.cpp)
target_link_libraries(TestJsonUtil sese)
add_test(NAME TestJsonUtil COMMAND TestJsonUtil)

# Http
add_executable(TestHttp ../test/TestHttp/main.cpp)
target_link_libraries(TestHttp sese)
add_test(NAME TestHttp COMMAND TestHttp)

# ObjectPool
add_executable(TestObjectPool ../test/TestObjectPool/main.cpp)
target_link_libraries(TestObjectPool sese)
add_test(NAME TestObjectPool COMMAND TestObjectPool)

# BufferedStream
add_executable(TestBufferedStream ../test/TestBufferedStream/main.cpp)
target_link_libraries(TestBufferedStream sese)
add_test(NAME TestBufferedStream COMMAND TestBufferedStream)

# Concurrent
add_executable(TestConcurrent ../test/TestConcurrent/main.cpp)
target_link_libraries(TestConcurrent sese)
add_test(NAME TestConcurrent COMMAND TestConcurrent)

# UrlHelper
add_executable(TestUrlHelper ../test/TestUrlHelper/main.cpp)
target_link_libraries(TestUrlHelper sese)
add_test(NAME TestUrlHelper COMMAND TestUrlHelper)

# Httpd
add_executable(TestHttpd ../test/TestHttpd/main.cpp)
target_link_libraries(TestHttpd sese)
add_test(NAME TestHttpd COMMAND TestHttpd)

# AddressPool
add_executable(TestAddressPool ../test/TestAddressPool/main.cpp)
target_link_libraries(TestAddressPool sese)
add_test(NAME TestAddressPool COMMAND TestAddressPool)

# HttpClient
add_executable(TestHttpClient ../test/TestHttpClient/main.cpp)
target_link_libraries(TestHttpClient sese)
add_test(NAME TestHttpClient COMMAND TestHttpClient)

# BlockAppender
add_executable(TestBlockAppender ../test/TestBlockAppender/main.cpp)
target_link_libraries(TestBlockAppender sese)
add_test(NAME TestBlockAppender COMMAND TestBlockAppender)

# TextReader
add_executable(TestTextReader ../test/TestTextReader/main.cpp)
target_link_libraries(TestTextReader sese)
add_test(NAME TestTextReader COMMAND TestTextReader)

# XmlUtil
add_executable(TestXmlUtil ../test/TestXmlUtil/main.cpp)
target_link_libraries(TestXmlUtil sese)
add_test(NAME TestXmlUtil COMMAND TestXmlUtil)

# Rpc
add_executable(TestRpc ../test/TestRpc/main.cpp)
target_link_libraries(TestRpc sese)
add_test(NAME TestRpc COMMAND TestRpc)

# SHA256Util
add_executable(TestSHA256Util ../test/TestSHA256Util/main.cpp)
target_link_libraries(TestSHA256Util sese)
add_test(NAME TestSHA256Util COMMAND TestSHA256Util)