find_package(ZLIB REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(SString CONFIG REQUIRED)
find_package(SimpleUuid CONFIG REQUIRED)
find_package(sese-plugin CONFIG REQUIRED)

set(
        RECORD_SRC
        record/AbstractAppender.cpp
        record/AsyncLogger.cpp
        record/BlockAppender.cpp
        record/ConsoleAppender.cpp
        record/FileAppender.cpp
        record/Logger.cpp
        record/LogHelper.cpp
        record/SimpleFormatter.cpp
)

set(
        CONVERT_SRC
        convert/Base64Converter.cpp
        convert/Compressor.cpp
        convert/Decompressor.cpp
        convert/EncodingConverter.cpp
        convert/GZipFileInputStream.cpp
        convert/GZipFileOutputStream.cpp
        convert/MD5Util.cpp
        convert/MessageDigest.cpp
        convert/PercentConverter.cpp
        convert/SHA1Util.cpp
        convert/SHA256Util.cpp
)

set(
        IO_SRC
        io/AbstractByteBuffer.cpp
        io/AbstractFixedBuffer.cpp
        io/BufferedInputStream.cpp
        io/BufferedOutputStream.cpp
        io/BufferedStream.cpp
        io/ByteBuffer.cpp
        io/ConsoleOutputStream.cpp
        io/FileStream.cpp
        io/FixedBuffer.cpp
        io/InputBufferWrapper.cpp
        io/InputStreamReader.cpp
        io/OutputBufferWrapper.cpp
)

set(
        THREAD_SRC
        thread/Thread.cpp
        thread/ThreadPool.cpp
)

set(
        SYSTEM_SRC
        system/Environment.cpp
        system/LibraryLoader.cpp
        system/NetworkUtil.cpp
)

set(
        NET_SRC
        net/Address.cpp
        net/AddressPool.cpp
        net/dns/Client.cpp
        net/dns/DNSServer.cpp
        net/dns/DNSUtil.cpp
        net/http/ContentTypeMap.cpp
        net/http/ControllerGroup.cpp
        net/http/Cookie.cpp
        net/http/CookieMap.cpp
        net/http/DynamicTable.cpp
        net/http/Header.cpp
        net/http/HPackUtil.cpp
        net/http/Http2Connection.cpp
        net/http/Http2Stream.cpp
        net/http/HttpClient.cpp
        net/http/HttpConnection.cpp
        net/http/HttpServer.cpp
        net/http/HttpUtil.cpp
        net/http/Huffman.cpp
        net/http/QueryString.cpp
        net/http/Range.cpp
        net/http/Request.cpp
        net/http/Response.cpp
        net/http/UrlHelper.cpp
        net/http/V2Http2Server.cpp
        net/http/V2HttpServer.cpp
        net/IPAddress.cpp
        net/IPv4Address.cpp
        net/IPv6Address.cpp
        net/ReusableSocket.cpp
        net/rpc/Client.cpp
        net/rpc/Server.cpp
        net/rpc/V2RpcServer.cpp
        net/Socket.cpp
        net/ws/WebSocketAuthenticator.cpp
)

set(
        CONFIG_SRC
        config/ConfigUtil.cpp
        config/CSVReader.cpp
        config/CSVWriter.cpp
        config/ini/IniUtil.cpp
        config/json/JsonTypes.cpp
        config/json/JsonUtil.cpp
        config/xml/XmlTypes.cpp
        config/xml/XmlUtil.cpp
        config/yaml/YamlTypes.cpp
        config/yaml/YamlUtil.cpp
)

set(
        TEXT_SRC
        text/AbstractStringBuffer.cpp
        text/DateTimeFormatter.cpp
        text/StringBuffer.cpp
        text/TextReader.cpp
)

set(
        UTIL_SRC
        util/ArgParser.cpp
        util/DateTime.cpp
        util/Initializer.cpp
        util/MemoryViewer.cpp
        util/Random.cpp
        util/Test.cpp
        util/Timer.cpp
        util/TimeSpan.cpp
        util/TimeWheel.cpp
        util/Util.cpp
        util/UuidBuilder.cpp
)

set(
        SECURITY_SRC
        security/MessageDigest.cpp
        security/SecurityConfig.cpp
        security/SecuritySocket.cpp
        security/SSLContext.cpp
        security/SSLContextBuilder.cpp
)

set(
        PLUGIN_SRC
        plugin/Module.cpp
)

set(
        SERVICE_SRC
        service/Http2Service_V1.cpp
        service/HttpConnection_V2.cpp
        service/HttpService_V1.cpp
        service/HttpService_V2.cpp
        service/iocp/IOBuf.cpp
        service/iocp/IOCPServer_V1.cpp
        service/RpcService.cpp
        service/SystemBalanceLoader.cpp
        service/TcpTransporter.cpp
        service/TimerableService_V1.cpp
        service/TimerableService_V2.cpp
        service/UserBalanceLoader.cpp
        service/WebsocketService.cpp
)

add_library(Core
        ${RECORD_SRC}
        ${CONVERT_SRC}
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

set(NATIVE_PATH "native/${CMAKE_SYSTEM_NAME}")
target_sources(Core PRIVATE
        ${NATIVE_PATH}/config/UniReader.cpp
        ${NATIVE_PATH}/net/ReadableServer.cpp
        ${NATIVE_PATH}/net/Socket.cpp
        ${NATIVE_PATH}/net/TcpServer.cpp
        ${NATIVE_PATH}/net/V2Server.cpp
        ${NATIVE_PATH}/security/evp/MD5Context.cpp
        ${NATIVE_PATH}/security/evp/SHA1Context.cpp
        ${NATIVE_PATH}/security/evp/SHA256Context.cpp
        ${NATIVE_PATH}/security/evp/SHA384Context.cpp
        ${NATIVE_PATH}/security/evp/SHA512Context.cpp
        ${NATIVE_PATH}/security/SecurityTcpServer.cpp
        ${NATIVE_PATH}/system/FileNotifier.cpp
        ${NATIVE_PATH}/system/Process.cpp
        ${NATIVE_PATH}/system/SharedMemory.cpp
        ${NATIVE_PATH}/system/StackInfo.cpp
        ${NATIVE_PATH}/Test.cpp
        ${NATIVE_PATH}/thread/GlobalThreadPool.cpp
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(DLL_INFO_RC ${PROJECT_BINARY_DIR}/WindowsDllInfo.rc)
    configure_file(${PROJECT_SOURCE_DIR}/WindowsDllInfo.rc.in ${DLL_INFO_RC})

    target_sources(Core PRIVATE ${DLL_INFO_RC})
    target_sources(Core PRIVATE ${NATIVE_PATH}/event/WSAEventLoop.cpp ${NATIVE_PATH}/event/WSAEventConvert.cpp ${NATIVE_PATH}/service/iocp/NativeIOCPServer_V1.cpp)

    if (${MSVC})
        target_compile_options(Core PRIVATE "/utf-8")
    endif ()
    target_link_libraries(Core PUBLIC crypt32 ws2_32 dbghelp iphlpapi advapi32 secur32)

    if (${MINGW})
        target_compile_options(Core PRIVATE "-fno-exceptions")
    else()
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

    target_sources(Core PRIVATE ${NATIVE_PATH}/event/EpollEventLoop.cpp ${NATIVE_PATH}/event/EpollEventConvert.cpp)

    target_compile_options(Core PUBLIC "-fPIC")
    target_link_options(Core PUBLIC "-rdynamic")

    target_compile_options(Core PRIVATE "-fno-exceptions")
    target_link_libraries(Core PRIVATE ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})
endif ()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    find_library(IOKIT_FRAMEWORK IOKit REQUIRED)
    find_library(CORESERVICES_FRAMEWORK CoreServices REQUIRED)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation REQUIRED)

    target_sources(Core PRIVATE ${NATIVE_PATH}/event/KqueueEventLoop.cpp)

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
        OpenSSL::Crypto
        OpenSSL::SSL
        ZLIB::ZLIB
)
target_link_libraries(Core PUBLIC
        Sese::SString
        Sese::Uuid
        Sese::Plugin
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
configure_package_config_file(
        ${PROJECT_SOURCE_DIR}/cmake/SeseConfig.cmake.in
        ${PROJECT_BINARY_DIR}/SeseConfig.cmake
        INSTALL_DESTINATION lib/cmake/sese.core
)

install(
        TARGETS Core # Core-static
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
        FILES "${PROJECT_BINARY_DIR}/SeseConfig.cmake"
        DESTINATION lib/cmake/sese
)

install(
        FILES "${PROJECT_BINARY_DIR}/SeseConfig.cmake"
        DESTINATION debug/lib/cmake/sese
)

install(
        EXPORT SeseTargets
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sese
        NAMESPACE Sese::
)