find_package(ZLIB REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(SString CONFIG REQUIRED)
find_package(SimpleUuid CONFIG REQUIRED)
find_package(sese-plugin CONFIG REQUIRED)
find_package(sese-event CONFIG REQUIRED)

add_library(Core SHARED)
# add_library(Core-static STATIC)

option(USE_ASYNC_LOGGER "use async logger" OFF)
if (USE_ASYNC_LOGGER)
    target_compile_definitions(Core PRIVATE -DUSE_ASYNC_LOGGER)
endif ()

target_include_directories(
        Core
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
# target_include_directories(
#         Core-static
#         PUBLIC
#         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
#         $<INSTALL_INTERFACE:include>
# )

set(
        RECORD_SRC
        src/record/AbstractAppender.cpp src/record/SimpleFormatter.cpp
        src/record/ConsoleAppender.cpp src/record/Logger.cpp
        src/record/LogHelper.cpp src/record/FileAppender.cpp
        src/record/BlockAppender.cpp src/record/AsyncLogger.cpp
)

set(
        CONVERT_SRC
        src/convert/EncodingConverter.cpp src/convert/Base64Converter.cpp
        src/convert/PercentConverter.cpp src/convert/MD5Util.cpp
        src/convert/SHA256Util.cpp src/convert/SHA1Util.cpp
        src/convert/MessageDigest.cpp src/convert/Compressor.cpp
        src/convert/Decompressor.cpp src/convert/GZipFileInputStream.cpp
        src/convert/GZipFileOutputStream.cpp
)

set(
        THREAD_SRC
        src/thread/Thread.cpp src/thread/ThreadPool.cpp
)

set(
        SYSTEM_SRC
        src/system/Environment.cpp src/system/LibraryLoader.cpp
        src/system/NetworkUtil.cpp
)

set(
        NET_SRC
        src/net/Address.cpp src/net/IPAddress.cpp
        src/net/IPv4Address.cpp src/net/IPv6Address.cpp
        src/net/AddressPool.cpp src/net/ReusableSocket.cpp
        src/net/Socket.cpp
        src/net/http/Header.cpp src/net/http/QueryString.cpp
        src/net/http/HttpUtil.cpp src/net/http/UrlHelper.cpp
        src/net/http/HttpClient.cpp src/net/http/HttpServer.cpp
        src/net/http/Cookie.cpp src/net/http/CookieMap.cpp
        src/net/http/DynamicTable.cpp src/net/http/Huffman.cpp
        src/net/http/V2HttpServer.cpp src/net/http/V2Http2Server.cpp
        src/net/http/Http2Connection.cpp src/net/http/Http2Stream.cpp
        src/net/http/HPackUtil.cpp
        src/net/rpc/V2RpcServer.cpp src/net/rpc/Server.cpp
        src/net/rpc/Client.cpp
        src/net/ws/WebSocketAuthenticator.cpp
)

set(
        CONFIG_SRC
        src/config/ConfigUtil.cpp
        src/config/json/JsonTypes.cpp src/config/json/JsonUtil.cpp
        src/config/xml/XmlTypes.cpp src/config/xml/XmlUtil.cpp
        src/config/CSVReader.cpp src/config/CSVWriter.cpp
        src/config/yaml/YamlTypes.cpp src/config/yaml/YamlUtil.cpp
)

set(
        TEXT_SRC
        src/text/DateTimeFormatter.cpp src/text/TextReader.cpp
        src/text/AbstractStringBuffer.cpp
)

set(
        UTIL_SRC
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
        src/util/AbstractFixedBuffer.cpp src/util/FixedBuffer.cpp
)

set(
        SECURITY_SRC
        src/security/SecurityConfig.cpp src/security/SSLContext.cpp
        src/security/SSLContextBuilder.cpp src/security/SecuritySocket.cpp
        src/security/MessageDigest.cpp
)

set(
        PLUGIN_SRC
        src/plugin/Module.cpp
)

set(
        SERVICE_SRC
        src/service/SystemBalanceLoader.cpp src/service/UserBalanceLoader.cpp
        src/service/TimerableService.cpp
)

target_sources(Core PRIVATE
        ${RECORD_SRC}
        ${CONVERT_SRC}
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

set(NATIVE_PATH "src/native/${CMAKE_SYSTEM_NAME}")
target_sources(Core PRIVATE
        ${NATIVE_PATH}/Test.cpp
        ${NATIVE_PATH}/net/Socket.cpp
        ${NATIVE_PATH}/net/TcpServer.cpp
        ${NATIVE_PATH}/net/ReadableServer.cpp
        ${NATIVE_PATH}/net/V2Server.cpp
        ${NATIVE_PATH}/security/SecurityTcpServer.cpp
        ${NATIVE_PATH}/config/UniReader.cpp
        ${NATIVE_PATH}/system/SharedMemory.cpp
        ${NATIVE_PATH}/system/FileNotifier.cpp
        ${NATIVE_PATH}/system/Process.cpp
        )

# target_sources(Core-static PRIVATE $<TARGET_OBJECTS:Core>)

if (WIN32)
    set(DLL_INFO_RC ${PROJECT_BINARY_DIR}/WindowsDllInfo.rc)
    configure_file(${PROJECT_SOURCE_DIR}/WindowsDllInfo.rc.in ${DLL_INFO_RC})

    target_sources(Core PRIVATE ${DLL_INFO_RC})
    target_compile_definitions(Core PUBLIC -DWINDOWS_DLL)
    target_compile_definitions(Core PUBLIC -DNEED_DBGHELP)
    target_compile_options(Core PRIVATE "/utf-8")
    target_compile_options(Core PRIVATE "/wd4251")
    target_compile_options(Core PRIVATE "/wd4530")
    target_link_libraries(Core PRIVATE ws2_32 iphlpapi)

    # target_compile_definitions(Core-static PUBLIC -DNEED_DBGHELP)
    # target_compile_options(Core-static PRIVATE "/utf-8")
    # target_compile_options(Core-static PRIVATE "/wd4251")
    # target_compile_options(Core-static PRIVATE "/wd4530")
    # target_link_libraries(Core-static PRIVATE ws2_32 iphlpapi)

    message(STATUS "Linked libraries: ws2_32, dbghelp, iphlpapi")

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
    target_link_libraries(Core PRIVATE ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})
    # target_link_libraries(Core-static PRIVATE ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})

    message(STATUS "Linked libraries: pthread, dl")
endif ()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    find_library(IOKIT_FRAMEWORK IOKit REQUIRED)
    find_library(CORESERVICES_FRAMEWORK CoreServices REQUIRED)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation REQUIRED)

    target_link_libraries(Core PRIVATE ${COREFOUNDATION_FRAMEWORK} ${IOKIT_FRAMEWORK} ${CORESERVICES_FRAMEWORK})
    # target_link_libraries(Core-static PRIVATE ${COREFOUNDATION_FRAMEWORK} ${IOKIT_FRAMEWORK} ${CORESERVICES_FRAMEWORK})

    message(STATUS "Linked framework: IOKit, CoreServices, CoreFoundation")
endif ()

set_target_properties(Core
        PROPERTIES
        CXX_STANDARD 17
        OUTPUT_NAME "sese.core"
        PREFIX ""
        WINDOWS_EXPORT_ALL_SYMBOLS ON
)

target_link_libraries(Core PRIVATE
        OpenSSL::Crypto
        OpenSSL::SSL
        ZLIB::ZLIB
        )
target_link_libraries(Core PUBLIC
        SString::SString-static
        SimpleUuid::uuid-static
        Sese::Plugin
        Sese::Event
        )

# target_link_libraries(Core-static PRIVATE
#         OpenSSL::Crypto
#         OpenSSL::SSL
#         ZLIB::ZLIB
#         )
# target_link_libraries(Core-static PUBLIC
#         SString::SString-static
#         SimpleUuid::uuid-static
#         Sese::Plugin
#         )

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