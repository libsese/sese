find_package(ZLIB REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(SString CONFIG REQUIRED)
find_package(SimpleUuid CONFIG REQUIRED)
find_package(sese-plugin CONFIG REQUIRED)
find_package(sese-event CONFIG REQUIRED)

set(
        RECORD_SRC
        src/record/AbstractAppender.cpp
        src/record/AsyncLogger.cpp
        src/record/BlockAppender.cpp
        src/record/ConsoleAppender.cpp
        src/record/FileAppender.cpp
        src/record/Logger.cpp
        src/record/LogHelper.cpp
        src/record/SimpleFormatter.cpp
)

set(
        CONVERT_SRC
        src/convert/Base64Converter.cpp
        src/convert/Compressor.cpp
        src/convert/Decompressor.cpp
        src/convert/EncodingConverter.cpp
        src/convert/GZipFileInputStream.cpp
        src/convert/GZipFileOutputStream.cpp
        src/convert/MD5Util.cpp
        src/convert/MessageDigest.cpp
        src/convert/PercentConverter.cpp
        src/convert/SHA1Util.cpp
        src/convert/SHA256Util.cpp
)

set(
        IO_SRC
        src/io/AbstractByteBuffer.cpp
        src/io/AbstractFixedBuffer.cpp
        src/io/BufferedInputStream.cpp
        src/io/BufferedOutputStream.cpp
        src/io/BufferedStream.cpp
        src/io/ByteBuffer.cpp
        src/io/ConsoleOutputStream.cpp
        src/io/FileStream.cpp
        src/io/FixedBuffer.cpp
        src/io/InputBufferWrapper.cpp
        src/io/InputStreamReader.cpp
        src/io/OutputBufferWrapper.cpp
)

set(
        THREAD_SRC
        src/thread/Thread.cpp
        src/thread/ThreadPool.cpp
)

set(
        SYSTEM_SRC
        src/system/Environment.cpp
        src/system/LibraryLoader.cpp
        src/system/NetworkUtil.cpp
)

set(
        NET_SRC
        src/net/Address.cpp
        src/net/AddressPool.cpp
        src/net/dns/Client.cpp
        src/net/dns/DNSServer.cpp
        src/net/dns/DNSUtil.cpp
        src/net/http/ContentTypeMap.cpp
        src/net/http/ControllerGroup.cpp
        src/net/http/Cookie.cpp
        src/net/http/CookieMap.cpp
        src/net/http/DynamicTable.cpp
        src/net/http/Header.cpp
        src/net/http/HPackUtil.cpp
        src/net/http/Http2Connection.cpp
        src/net/http/Http2Stream.cpp
        src/net/http/HttpClient.cpp
        src/net/http/HttpConnection.cpp
        src/net/http/HttpServer.cpp
        src/net/http/HttpUtil.cpp
        src/net/http/Huffman.cpp
        src/net/http/QueryString.cpp
        src/net/http/Range.cpp
        src/net/http/Request.cpp
        src/net/http/Response.cpp
        src/net/http/UrlHelper.cpp
        src/net/http/V2Http2Server.cpp
        src/net/http/V2HttpServer.cpp
        src/net/IPAddress.cpp
        src/net/IPv4Address.cpp
        src/net/IPv6Address.cpp
        src/net/ReusableSocket.cpp
        src/net/rpc/Client.cpp
        src/net/rpc/Server.cpp
        src/net/rpc/V2RpcServer.cpp
        src/net/Socket.cpp
        src/net/ws/WebSocketAuthenticator.cpp
)

set(
        CONFIG_SRC
        src/config/ConfigUtil.cpp
        src/config/CSVReader.cpp
        src/config/CSVWriter.cpp
        src/config/ini/IniUtil.cpp
        src/config/json/JsonTypes.cpp
        src/config/json/JsonUtil.cpp
        src/config/xml/XmlTypes.cpp
        src/config/xml/XmlUtil.cpp
        src/config/yaml/YamlTypes.cpp
        src/config/yaml/YamlUtil.cpp
)

set(
        TEXT_SRC
        src/text/AbstractStringBuffer.cpp
        src/text/DateTimeFormatter.cpp
        src/text/StringBuffer.cpp
        src/text/TextReader.cpp
)

set(
        UTIL_SRC
        src/util/ArgParser.cpp
        src/util/DateTime.cpp
        src/util/Initializer.cpp
        src/util/MemoryViewer.cpp
        src/util/Random.cpp
        src/util/Test.cpp
        src/util/Timer.cpp
        src/util/TimeSpan.cpp
        src/util/TimeWheel.cpp
        src/util/Util.cpp
        src/util/UuidBuilder.cpp
)

set(
        SECURITY_SRC
        src/security/MessageDigest.cpp
        src/security/SecurityConfig.cpp
        src/security/SecuritySocket.cpp
        src/security/SSLContext.cpp
        src/security/SSLContextBuilder.cpp
)

set(
        PLUGIN_SRC
        src/plugin/Module.cpp
)

set(
        SERVICE_SRC
        src/service/Http2Service_V1.cpp
        src/service/HttpConnection_V2.cpp
        src/service/HttpService_V1.cpp
        src/service/HttpService_V2.cpp
        src/service/RpcService.cpp
        src/service/SystemBalanceLoader.cpp
        src/service/TcpTransporter.cpp
        src/service/TimerableService_V1.cpp
        src/service/UserBalanceLoader.cpp
        src/service/WebsocketService.cpp
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

target_compile_definitions(Core PUBLIC -DSESE_USE_SSL)

target_include_directories(
        Core
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

set(NATIVE_PATH "src/native/${CMAKE_SYSTEM_NAME}")
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

    target_compile_options(Core PUBLIC "-fPIC")
    target_link_options(Core PUBLIC "-rdynamic")

    target_compile_options(Core PRIVATE "-fno-exceptions")
    target_link_libraries(Core PRIVATE ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})
endif ()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    find_library(IOKIT_FRAMEWORK IOKit REQUIRED)
    find_library(CORESERVICES_FRAMEWORK CoreServices REQUIRED)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation REQUIRED)

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
        Sese::Event
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
        DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/include/sese"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
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