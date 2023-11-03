find_package(ZLIB REQUIRED)
find_package(OpenSSL REQUIRED)

add_library(Plugin STATIC
        plugin/BaseClass.h
        plugin/ClassFactory.cpp
        plugin/ClassFactory.h
        plugin/Marco.h
        plugin/ModuleInfo.h
)

set(CONCURRENT_SRC
        concurrent/CASDefine.h
        concurrent/ObjectPool.h
        concurrent/LinkedQueue.h
        concurrent/LinkedStack.h
)

set(EVENT_SRC
        event/BaseEvent.h
        event/BaseEventConvert.h
        event/BaseEventLoop.h
        event/Event.h
)

set(
        RECORD_SRC
        record/AbstractAppender.cpp
        record/AbstractAppender.h
        record/AbstractFormatter.h
        record/AsyncLogger.cpp
        record/AsyncLogger.h
        record/BlockAppender.cpp
        record/BlockAppender.h
        record/ConsoleAppender.cpp
        record/ConsoleAppender.h
        record/FileAppender.cpp
        record/FileAppender.h
        record/Event.h
        record/Logger.cpp
        record/Logger.h
        record/LogHelper.cpp
        record/LogHelper.h
        record/Marco.h
        record/SimpleFormatter.cpp
        record/SimpleFormatter.h
)

set(
        CONVERT_SRC
        convert/Base64Converter.cpp
        convert/Base64Converter.h
        convert/Compressor.cpp
        convert/Compressor.h
        convert/Decompressor.cpp
        convert/Decompressor.h
        convert/EncodingConverter.cpp
        convert/EncodingConverter.h
        convert/GZipFileInputStream.cpp
        convert/GZipFileInputStream.h
        convert/GZipFileOutputStream.cpp
        convert/GZipFileOutputStream.h
        convert/MD5Util.cpp
        convert/MD5Util.h
        convert/MessageDigest.cpp
        convert/MessageDigest.h
        convert/PercentConverter.cpp
        convert/PercentConverter.h
        convert/SHA1Util.cpp
        convert/SHA1Util.h
        convert/SHA256Util.cpp
        convert/SHA256Util.h
        convert/ZlibConfig.h
)

set(
        IO_SRC
        io/AbstractByteBuffer.cpp
        io/AbstractByteBuffer.h
        io/AbstractFixedBuffer.cpp
        io/AbstractFixedBuffer.h
        io/BaseStreamReader.h
        io/BufferedInputStream.cpp
        io/BufferedInputStream.h
        io/BufferedOutputStream.cpp
        io/BufferedOutputStream.h
        io/BufferedStream.cpp
        io/BufferedStream.h
        io/ByteBuffer.cpp
        io/ByteBuffer.h
        io/ByteBuilder.h
        io/Closeable.h
        io/ConsoleOutputStream.cpp
        io/ConsoleOutputStream.h
        io/FakeStream.h
        io/File.h
        io/FileStream.cpp
        io/FileStream.h
        io/FixedBuffer.cpp
        io/FixedBuffer.h
        io/FixedBuilder.h
        io/InputBufferWrapper.cpp
        io/InputBufferWrapper.h
        io/InputStream.h
        io/InputStreamReader.cpp
        io/InputStreamReader.h
        io/NullOutputStream.cpp
        io/NullOutputStream.h
        io/OutputBufferWrapper.cpp
        io/OutputBufferWrapper.h
        io/OutputStream.h
        io/OutputUtil.h
        io/PeekableStream.h
        io/RandomInputStream.cpp
        io/RandomInputStream.h
        io/Stream.h
)

set(
        THREAD_SRC
        thread/Async.h
        thread/GlobalThreadPool.h
        thread/Locker.h
        thread/SpinLock.cpp
        thread/SpinLock.h
        thread/Thread.cpp
        thread/Thread.h
        thread/ThreadPool.cpp
        thread/ThreadPool.h
)

set(
        SYSTEM_SRC
        system/CommandLine.cpp
        system/CommandLine.h
        system/Environment.cpp
        system/Environment.h
        system/FileLocker.h
        system/FileNotifier.h
        system/LibraryLoader.h
        system/NetworkUtil.cpp
        system/NetworkUtil.h
        system/Path.h
        system/Process.h
        system/SharedMemory.h
        system/StackInfo.h
)

set(
        NET_SRC
        net/Address.cpp
        net/Address.h
        net/AddressPool.cpp
        net/AddressPool.h
        net/dns/Answer.h
        net/dns/DnsClient.cpp
        net/dns/DnsClient.h
        net/dns/DnsServer.cpp
        net/dns/DnsServer.h
        net/dns/DndSession.h
        net/dns/DnsUtil.cpp
        net/dns/DnsUtil.h
        net/dns/FrameHeader.h
        net/dns/Query.h
        net/http/ContentTypeMap.cpp
        net/http/ControllerGroup.cpp
        net/http/ControllerGroup.h
        net/http/Cookie.cpp
        net/http/Cookie.h
        net/http/CookieMap.cpp
        net/http/CookieMap.h
        net/http/DynamicTable.cpp
        net/http/DynamicTable.h
        net/http/Header.cpp
        net/http/Header.h
        net/http/HeaderBuilder.cpp
        net/http/HeaderBuilder.h
        net/http/HPACK.h
        net/http/HPackUtil.cpp
        net/http/HPackUtil.h
        net/http/Http2FrameInfo.h
        net/http/HttpClient.cpp
        net/http/HttpClient.h
        net/http/HttpUtil.cpp
        net/http/HttpUtil.h
        net/http/Huffman.cpp
        net/http/Huffman.h
        net/http/QueryString.cpp
        net/http/QueryString.h
        net/http/Range.cpp
        net/http/Range.h
        net/http/Request.cpp
        net/http/Request.h
        net/http/RequestHeader.h
        net/http/RequestParser.cpp
        net/http/RequestParser.h
        net/http/Response.cpp
        net/http/Response.h
        net/http/ResponseHeader.h
        net/http/UrlHelper.cpp
        net/http/UrlHelper.h
        net/IPAddress.cpp
        net/IPAddress.h
        net/IPv4Address.cpp
        net/IPv4Address.h
        net/IPv6Address.cpp
        net/IPv6Address.h
        net/ReadableServer.h
        net/ReusableSocket.cpp
        net/ReusableSocket.h
        net/rpc/RpcClient.cpp
        net/rpc/RpcClient.h
        net/rpc/Marco.h
        net/rpc/Server.cpp
        net/rpc/Server.h
        net/Socket.cpp
        net/Socket.h
        net/ws/FrameHeader.h
        net/ws/WebSocketAuthenticator.cpp
        net/ws/WebsocketAuthenticator.h
        net/ws/WebsocketEvent.h
        net/ws/WebsocketSession.h
)

set(
        CONFIG_SRC
        config/ConfigUtil.cpp
        config/ConfigUtil.h
        config/CSVReader.cpp
        config/CSVReader.h
        config/CSVWriter.cpp
        config/CSVWriter.h
        config/ini/IniConfig.h
        config/ini/IniUtil.cpp
        config/ini/IniUtil.h
        config/json/JsonTypes.cpp
        config/json/JsonTypes.h
        config/json/JsonUtil.cpp
        config/json/JsonUtil.h
        config/json/Marco.h
        config/json.cpp
        config/json.h
        config/UniReader.h
        config/xml/XmlTypes.cpp
        config/xml/XmlTypes.h
        config/xml/XmlUtil.cpp
        config/xml/XmlUtil.h
        config/yaml/Marco.h
        config/yaml/YamlTypes.cpp
        config/yaml/YamlTypes.h
        config/yaml/YamlUtil.cpp
        config/yaml/YamlUtil.h
)

set(
        TEXT_SRC
        text/AbstractStringBuffer.cpp
        text/AbstractStringBuffer.h
        text/Algorithm.cpp
        text/Algorithm.h
        text/DateTimeFormatter.cpp
        text/DateTimeFormatter.h
        text/Number.cpp
        text/Number.h
        text/SString.cpp
        text/SString.h
        text/SStringBuilder.cpp
        text/SStringBuilder.h
        text/String.h
        text/StringBuffer.cpp
        text/StringBuffer.h
        text/StringBuilder.cpp
        text/StringBuilder.h
        text/TextReader.cpp
        text/TextReader.h
)

set(
        UTIL_SRC
        util/ArgParser.cpp
        util/ArgParser.h
        util/DateTime.cpp
        util/DateTime.h
        util/Endian.h
        util/Exception.cpp
        util/Exception.h
        util/Initializer.cpp
        util/Initializer.h
        util/Memory.h
        util/MemoryViewer.cpp
        util/MemoryViewer.h
        util/Noncopyable.h
        util/NotInstantiable.h
        util/ObjectPool.h
        util/Random.cpp
        util/Random.h
        util/RandomUtil.cpp
        util/RandomUtil.h
        util/Range.h
        util/Singleton.h
        util/StopWatch.cpp
        util/StopWatch.h
        util/Timer.cpp
        util/Timer.h
        util/TimeSpan.cpp
        util/TimeSpan.h
        util/TimestampHandler.cpp
        util/TimestampHandler.h
        util/TimeWheel.cpp
        util/TimeWheel.h
        util/Util.cpp
        util/Util.h
        util/Uuid.cpp
        util/Uuid.h
        util/UuidBuilder.cpp
        util/UuidBuilder.h
        util/Value.cpp
        util/Value.h
        util/Version.cpp
        util/Version.h
)

set(
        SECURITY_SRC
        security/evp/Context.h
        security/evp/MD5Context.h
        security/evp/SHA1Context.h
        security/evp/SHA256Context.h
        security/evp/SHA384Context.h
        security/evp/SHA512Context.h
        security/MessageDigest.cpp
        security/MessageDigest.h
        security/SecurityConfig.cpp
        security/SecurityConfig.h
        security/SecuritySocket.cpp
        security/SecuritySocket.h
        security/SSLContext.cpp
        security/SSLContext.h
        security/SSLContextBuilder.cpp
        security/SSLContextBuilder.h
)

set(
        PLUGIN_SRC
        plugin/Module.cpp
        plugin/Module.h
)

set(
        SERVICE_SRC
        service/BalanceLoader.h
        service/http/HttpClient.h
        service/http/HttpClient_V1.cpp
        service/http/HttpClient_V1.h
        service/http/HttpClientHandle_V1.cpp
        service/http/HttpClientHandle_V1.h
        service/http/HttpConnection_V2.cpp
        service/http/HttpConnection_V2.h
        service/http/HttpService.h
        service/http/HttpService_V2.cpp
        service/http/HttpService_V2.h
        service/iocp/IOBuf.cpp
        service/iocp/IOBuf.h
        service/iocp/IOCPServer.h
        service/iocp/IOCPServer_V1.cpp
        service/iocp/IOCPServer_V1.h
        service/rpc/RpcService.h
        service/rpc/RpcService_V1.cpp
        service/rpc/RpcService_V1.h
        service/SystemBalanceLoader.cpp
        service/SystemBalanceLoader.h
        service/TcpTransporter.cpp
        service/TcpTransporter.h
        service/TimerableService.h
        service/TimerableService_V1.cpp
        service/TimerableService_V1.h
        service/TimerableService_V2.cpp
        service/TimerableService_V2.h
        service/UserBalanceLoader.cpp
        service/UserBalanceLoader.h
)

set(
        UNIX_SRC
        native/unix/config/UniReader.cpp
        native/unix/net/Socket.cpp
        native/unix/security/evp/MD5Context.cpp
        native/unix/security/evp/SHA1Context.cpp
        native/unix/security/evp/SHA256Context.cpp
        native/unix/security/evp/SHA384Context.cpp
        native/unix/security/evp/SHA512Context.cpp
        native/unix/system/FileLocker.cpp
        native/unix/system/LibraryLoader.cpp
        native/unix/system/Path.cpp
        native/unix/system/Process.cpp
        native/unix/system/SharedMemory.cpp
        native/unix/thread/GlobalThreadPool.cpp
)

add_library(Core
        ${CONCURRENT_SRC}
        ${CONVERT_SRC}
        ${EVENT_SRC}
        ${RECORD_SRC}
        ${IO_SRC}
        ${THREAD_SRC}
        ${SYSTEM_SRC}
        ${NET_SRC}
        ${CONFIG_SRC}
        ${TEXT_SRC}
        ${UTIL_SRC}
        ${SECURITY_SRC}
        ${PLUGIN_SRC}
        ${SERVICE_SRC}
        Config.h
)

option(USE_ASYNC_LOGGER "use async logger" OFF)
if (USE_ASYNC_LOGGER)
    target_compile_definitions(Core PRIVATE -DUSE_ASYNC_LOGGER)
endif ()

if (SESE_BUILD_TEST)
    target_compile_definitions(Core PUBLIC -DSESE_BUILD_TEST)
endif ()

target_include_directories(
        Core
        PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_include_directories(
        Plugin
        PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(DLL_INFO_RC ${PROJECT_BINARY_DIR}/WindowsDllInfo.rc)
    configure_file(${PROJECT_SOURCE_DIR}/WindowsDllInfo.rc.in ${DLL_INFO_RC})

    target_sources(Core PRIVATE ${DLL_INFO_RC})
    target_sources(Core PRIVATE
            native/win/event/WSAEvent.h
            native/win/event/WSAEventLoop.cpp
            native/win/event/WSAEventLoop.h
            native/win/event/WSAEventConvert.cpp
            native/win/event/WSAEventConvert.h
            native/win/service/iocp/NativeIOCPServer_V1.cpp
            native/win/service/iocp/NativeIOCPServer_V1.h
            native/win/system/FileNotifier.cpp
            native/win/system/StackInfo.cpp
    )
    target_sources(Core PRIVATE
            native/win/Config.h
            native/win/config/UniReader.cpp
            native/win/net/Socket.cpp
            native/win/security/evp/MD5Context.cpp
            native/win/security/evp/SHA1Context.cpp
            native/win/security/evp/SHA256Context.cpp
            native/win/security/evp/SHA384Context.cpp
            native/win/security/evp/SHA512Context.cpp
            native/win/system/FileLocker.cpp
            native/win/system/LibraryLoader.cpp
            native/win/system/Path.cpp
            native/win/system/Process.cpp
            native/win/system/SharedMemory.cpp
            native/win/thread/GlobalThreadPool.cpp
    )

    if (${MSVC})
        target_compile_options(Core PRIVATE "/utf-8")
        target_compile_options(Plugin PRIVATE "/utf-8")
    endif ()
    target_link_libraries(Core PUBLIC crypt32 ws2_32 dbghelp iphlpapi advapi32 secur32)

    if (${MINGW})
        target_compile_options(Core PRIVATE "-fno-exceptions")
    else ()
        target_compile_options(Core PRIVATE "/D_HAS_EXCEPTIONS=0")
    endif ()

    if (${CMAKE_GENERATOR} MATCHES "Ninja")
        # 单配置
        add_custom_command(
                TARGET
                Core
                POST_BUILD
                COMMAND
                ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:Core> ${PROJECT_BINARY_DIR}/gtest
        )
    else ()
        # 多配置
        add_custom_command(
                TARGET
                Core
                POST_BUILD
                COMMAND
                ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:Core> ${PROJECT_BINARY_DIR}/gtest/$<CONFIG>
        )
    endif ()
endif ()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_package(Threads REQUIRED)

    target_sources(Core PRIVATE
            native/linux/event/EpollEvent.h
            native/linux/event/EpollEventLoop.cpp
            native/linux/event/EpollEventLoop.h
            native/linux/event/EpollEventConvert.cpp
            native/linux/event/EpollEventConvert.h
    )
    target_sources(Core PRIVATE ${UNIX_SRC})
    target_sources(Core PRIVATE
            native/linux/Config.h
            native/linux/system/FileNotifier.cpp
            native/linux/system/StackInfo.cpp
    )

    target_compile_options(Plugin PUBLIC "-fPIC")

    target_compile_options(Core PUBLIC "-fPIC")
    target_link_options(Core PUBLIC "-rdynamic")

    target_compile_options(Core PRIVATE "-fno-exceptions")
    target_link_libraries(Core PRIVATE ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})
endif ()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    find_library(IOKIT_FRAMEWORK IOKit REQUIRED)
    find_library(CORESERVICES_FRAMEWORK CoreServices REQUIRED)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation REQUIRED)

    target_compile_options(Plugin PUBLIC "-fPIC")

    target_sources(Core PRIVATE
            native/darwin/event/KqueueEvent.h
            native/darwin/event/KqueueEventLoop.cpp
            native/darwin/event/KqueueEventLoop.h
    )
    target_sources(Core PRIVATE ${UNIX_SRC})
    target_sources(Core PRIVATE
            native/darwin/Config.h
            native/darwin/system/FileNotifier.cpp
            native/darwin/system/StackInfo.cpp
    )

    target_compile_options(Core PRIVATE "-fno-exceptions")
    target_link_libraries(Core PRIVATE ${COREFOUNDATION_FRAMEWORK} ${IOKIT_FRAMEWORK} ${CORESERVICES_FRAMEWORK})
endif ()

set_target_properties(Core
        PROPERTIES
        CXX_STANDARD 17
        OUTPUT_NAME "sese.core"
        PREFIX ""
        WINDOWS_EXPORT_ALL_SYMBOLS ON
)

target_link_libraries(Core PUBLIC
        Plugin
)

target_link_libraries(Core PRIVATE
        OpenSSL::Crypto
        OpenSSL::SSL
        ZLIB::ZLIB
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
configure_package_config_file(
        ${PROJECT_SOURCE_DIR}/cmake/sese-config.cmake.in
        ${PROJECT_BINARY_DIR}/sese-config.cmake
        INSTALL_DESTINATION lib/cmake/sese-core
)

install(
        TARGETS Core Plugin
        EXPORT SeseTargets
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include
)

install(
        DIRECTORY "${PROJECT_SOURCE_DIR}/sese"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        FILES_MATCHING
        PATTERN "*.h"
)

install(
        FILES "${PROJECT_BINARY_DIR}/sese-config.cmake"
        DESTINATION lib/cmake/sese-core
)

install(
        FILES "${PROJECT_BINARY_DIR}/sese-config.cmake"
        DESTINATION debug/lib/cmake/sese-core
)

install(
        EXPORT SeseTargets
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sese-core
        NAMESPACE Sese::
)