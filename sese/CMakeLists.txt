add_library(sese SHARED)
add_library(sese-static STATIC)

target_include_directories(sese PUBLIC ${PROJECT_SOURCE_DIR})
target_include_directories(sese-static PUBLIC ${PROJECT_SOURCE_DIR})

target_sources(sese PRIVATE
        record/AbstractAppender.cpp record/SimpleFormatter.cpp
        record/ConsoleAppender.cpp record/Logger.cpp
        record/LogHelper.cpp record/FileAppender.cpp
        record/BlockAppender.cpp

        convert/EncodingConverter.cpp convert/Base64Converter.cpp
        convert/PercentConverter.cpp convert/MD5Util.cpp
        convert/SHA256Util.cpp convert/SHA1Util.cpp
        convert/MessageDigest.cpp

        thread/Thread.cpp thread/ThreadPool.cpp

        system/Environment.cpp system/LibraryLoader.cpp

        net/Address.cpp net/IPAddress.cpp
        net/IPv4Address.cpp net/IPv6Address.cpp
        net/AddressPool.cpp
        net/http/Header.cpp net/http/QueryString.cpp
        net/http/HttpUtil.cpp net/http/UrlHelper.cpp
        net/http/HttpClient.cpp net/http/HttpServer.cpp
        net/http/Cookie.cpp net/http/CookieMap.cpp
        net/rpc/Server.cpp net/rpc/Client.cpp
        net/ws/WebSocketContext.cpp net/ws/WebSocketListener.cpp
        net/ws/WebSocketAuthenticator.cpp

        config/ConfigUtil.cpp
        config/json/JsonTypes.cpp config/json/JsonUtil.cpp
        config/xml/XmlTypes.cpp config/xml/XmlUtil.cpp

        text/DateTimeFormatter.cpp text/TextReader.cpp
        text/AbstractStringBuffer.cpp

        util/ArgParser.cpp util/UuidBuilder.cpp
        util/AbstractByteBuffer.cpp util/BufferedStream.cpp
        util/BufferedOutputStream.cpp util/BufferedInputStream.cpp
        util/DateTime.cpp util/TimeSpan.cpp
        util/StringBuffer.cpp util/ByteBuffer.cpp
        util/Random.cpp util/Util.cpp
        util/FileStream.cpp util/Test.cpp
        util/Timer.cpp util/Initializer.cpp
        util/MemoryViewer.cpp util/ConsoleOutputStream.cpp
        util/OutputBufferWrapper.cpp util/InputBufferWrapper.cpp
        )

target_compile_definitions(sese PRIVATE -DSESE_USE_SSL)
target_compile_definitions(sese-static PRIVATE -DSESE_USE_SSL)
target_sources(sese PRIVATE
        security/SecurityConfig.cpp security/SSLContext.cpp
        security/SSLContextBuilder.cpp security/SecuritySocket.cpp
        )
target_link_libraries(sese crypto ssl)
target_link_libraries(sese-static crypto ssl)

set(NATIVE_PATH "native/${CMAKE_SYSTEM_NAME}")
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