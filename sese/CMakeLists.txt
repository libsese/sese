add_library(sese SHARED)
add_library(sese-static STATIC)

target_include_directories(sese PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_include_directories(sese-static PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

target_include_directories(sese PRIVATE ${LibreSSL_SOURCE_DIR}/include)
target_include_directories(sese-static PRIVATE ${LibreSSL_SOURCE_DIR}/include)

target_sources(sese PRIVATE
        src/record/AbstractAppender.cpp src/record/SimpleFormatter.cpp
        src/record/ConsoleAppender.cpp src/record/Logger.cpp
        src/record/LogHelper.cpp src/record/FileAppender.cpp
        src/record/BlockAppender.cpp

        src/convert/EncodingConverter.cpp src/convert/Base64Converter.cpp
        src/convert/PercentConverter.cpp src/convert/MD5Util.cpp
        src/convert/SHA256Util.cpp src/convert/SHA1Util.cpp
        src/convert/MessageDigest.cpp

        src/thread/Thread.cpp src/thread/ThreadPool.cpp

        src/system/Environment.cpp src/system/LibraryLoader.cpp

        src/net/Address.cpp src/net/IPAddress.cpp
        src/net/IPv4Address.cpp src/net/IPv6Address.cpp
        src/net/AddressPool.cpp
        src/net/http/Header.cpp src/net/http/QueryString.cpp
        src/net/http/HttpUtil.cpp src/net/http/UrlHelper.cpp
        src/net/http/HttpClient.cpp src/net/http/HttpServer.cpp
        src/net/http/Cookie.cpp src/net/http/CookieMap.cpp
        src/net/rpc/Server.cpp src/net/rpc/Client.cpp
        src/net/ws/WebSocketContext.cpp src/net/ws/WebSocketAuthenticator.cpp

        src/config/ConfigUtil.cpp
        src/config/json/JsonTypes.cpp src/config/json/JsonUtil.cpp
        src/config/xml/XmlTypes.cpp src/config/xml/XmlUtil.cpp

        src/text/DateTimeFormatter.cpp src/text/TextReader.cpp
        src/text/AbstractStringBuffer.cpp

        src/util/ArgParser.cpp src/util/UuidBuilder.cpp
        src/util/AbstractByteBuffer.cpp src/util/BufferedStream.cpp
        src/util/BufferedOutputStream.cpp src/util/BufferedInputStream.cpp
        src/util/DateTime.cpp src/util/TimeSpan.cpp
        src/util/StringBuffer.cpp src/util/ByteBuffer.cpp
        src/util/Random.cpp src/util/Util.cpp
        src/util/FileStream.cpp src/util/Test.cpp
        src/util/Timer.cpp src/util/Initializer.cpp
        src/util/MemoryViewer.cpp src/util/ConsoleOutputStream.cpp
        src/util/OutputBufferWrapper.cpp src/util/InputBufferWrapper.cpp
        )

target_compile_definitions(sese PRIVATE -DSESE_USE_SSL)
target_compile_definitions(sese-static PRIVATE -DSESE_USE_SSL)
target_sources(sese PRIVATE
        src/security/SecurityConfig.cpp src/security/SSLContext.cpp
        src/security/SSLContextBuilder.cpp src/security/SecuritySocket.cpp
        )
if (WIN32)
    target_link_libraries(sese crypto-static ssl-static tls-static)
    target_link_libraries(sese-static crypto-static ssl-static tls-static)
else()
    target_link_libraries(sese crypto_obj ssl_obj tls_obj)
    target_link_libraries(sese-static crypto_obj ssl_obj tls_obj)
endif()

set(NATIVE_PATH "src/native/${CMAKE_SYSTEM_NAME}")
target_sources(sese PRIVATE
        ${NATIVE_PATH}/Test.cpp
        ${NATIVE_PATH}/net/Socket.cpp
        ${NATIVE_PATH}/net/TcpServer.cpp
        ${NATIVE_PATH}/net/ReadableServer.cpp
        ${NATIVE_PATH}/security/SecurityTcpServer.cpp
        ${NATIVE_PATH}/config/UniReader.cpp
        ${NATIVE_PATH}/system/SharedMemory.cpp
        )

target_sources(sese-static PRIVATE $<TARGET_OBJECTS:sese>)

if (WIN32)
    set(DLL_INFO_RC ${PROJECT_BINARY_DIR}/WindowsDllInfo.rc)
    configure_file(${PROJECT_SOURCE_DIR}/WindowsDllInfo.rc.in ${DLL_INFO_RC})

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

target_link_libraries(sese SString-static uuid-static)
target_link_libraries(sese-static SString-static uuid-static)

set_target_properties(sese PROPERTIES OUTPUT_NAME "sese.core" PREFIX "")