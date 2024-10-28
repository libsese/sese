list(APPEND CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/cmake")

if (SESE_BUILD_TEST)
    enable_testing()
    add_subdirectory(benchmark)
    add_subdirectory(test)
endif ()

if (SESE_BUILD_EXAMPLE)
    add_subdirectory(example)
endif ()

###################################################################################
# 通用目标
# 1.Core
###################################################################################
file(
        GLOB_RECURSE PUBLIC_SRC
        "concurrent/*.cpp" "concurrent/*.h"
        "event/*.h"
        "record/*.cpp" "record/*.h"
        "convert/*.cpp" "convert/*.h"
        "io/*.cpp" "io/*.h"
        "thread/*.cpp" "thread/*.h"
        "system/*.cpp" "system/*.h"
        "net/*.cpp" "net/*.h"
        "config/*.cpp" "config/*.h"
        "text/*.cpp" "text/*.h"
        "util/*.cpp" "util/*.h"
        "security/*.cpp" "security/*.h"
        "plugin/*.cpp" "plugin/*.h"
        "service/*.cpp" "service/*.h"
        "res/*.cpp" "res/*.h"
        "container/*.cpp" "container/*.h"
)

file(
        GLOB_RECURSE UNIX_SRC
        "native/unix/*.cpp" "native/unix/*.h"
)

file(
        GLOB_RECURSE INTERNAL_SRC
        "internal/net/*.cpp"
        "internal/net/*.h"
        "internal/service/*.cpp"
        "internal/service/*.h"
)

add_library(Core
        ${PUBLIC_SRC}
        ${INTERNAL_SRC}
        Config.h
        Util.h
)

if (SESE_BUILD_TEST)
    target_compile_definitions(Core PUBLIC -DSESE_BUILD_TEST)
endif ()

target_include_directories(
        Core
        PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

set_target_properties(
        Core PROPERTIES
        CXX_STANDARD 20
        OUTPUT_NAME "sese.core"
        PREFIX ""
        ENABLE_EXPORTS ON
        WINDOWS_EXPORT_ALL_SYMBOLS ON
)

if (MSVC AND NOT ${CMAKE_BUILD_TYPE} MATCHES "Debug")
    target_compile_options(Core PRIVATE /Zi)
    set_target_properties(Core PROPERTIES
            LINK_FLAGS "/DEBUG /OPT:REF /OPT:ICF"
    )
endif ()

target_compile_definitions(Core PRIVATE "SESE_MAJOR_VERSION=\"${PROJECT_VERSION_MAJOR}\"")
target_compile_definitions(Core PRIVATE "SESE_MINOR_VERSION=\"${PROJECT_VERSION_MINOR}\"")
target_compile_definitions(Core PRIVATE "SESE_PATCH_VERSION=\"${PROJECT_VERSION_PATCH}\"")
target_compile_definitions(Core PRIVATE "SESE_REPO_HASH=\"${SESE_REPO_HASH}\"")
target_compile_definitions(Core PRIVATE "SESE_REPO_BRANCH=\"${SESE_REPO_BRANCH}\"")

find_package(asio CONFIG REQUIRED)
target_link_libraries(Core PRIVATE asio::asio)

if (SESE_REPLACE_EXECINFO)
    find_package(PkgConfig)
    pkg_check_modules(libunwind REQUIRED IMPORTED_TARGET GLOBAL libunwind)
    target_link_libraries(Core PRIVATE PkgConfig::libunwind)
endif ()

find_package(OpenSSL REQUIRED)
target_link_libraries(Core PRIVATE OpenSSL::Crypto OpenSSL::SSL)

find_package(ZLIB REQUIRED)
target_link_libraries(Core PRIVATE ZLIB::ZLIB)

###################################################################################
# 内部独立子库和可选项
# 1.Archive
# 2.DB
# 3.Async Logger
# 4.SESE_RECORD_TIME_PATTERN
# 5.SESE_RECORD_TEXT_PATTERN
# 6.Replace execinfo
###################################################################################
if (MSVC)
    add_definitions(/utf-8)
endif ()
include_directories(${PROJECT_SOURCE_DIR})

if (SESE_USE_ARCHIVE)
    add_subdirectory(archive)
    target_link_libraries(Core PRIVATE Archive)
    set(ARCHIVE_TARGET Archive)
endif ()

add_subdirectory(db)
target_link_libraries(Core PRIVATE DB)

if (SESE_USE_ASYNC_LOGGER)
    target_compile_definitions(Core PRIVATE -DUSE_ASYNC_LOGGER)
endif ()

set(SESE_RECORD_TIME_PATTERN $ENV{SESE_RECORD_TIME_PATTERN})
if (NOT SESE_RECORD_TIME_PATTERN STREQUAL "")
    target_compile_definitions(
            Core PRIVATE
            "SESE_RECORD_TIME_PATTERN=\"${SESE_RECORD_TIME_PATTERN}\""
    )
endif ()

set(SESE_RECORD_TEXT_PATTERN $ENV{SESE_RECORD_TEXT_PATTERN})
if (NOT SESE_RECORD_TEXT_PATTERN STREQUAL "")
    target_compile_definitions(
            Core PRIVATE
            "SESE_RECORD_TEXT_PATTERN=\"${SESE_RECORD_TEXT_PATTERN}\""
    )
endif ()

if (SESE_REPLACE_EXECINFO)
    target_compile_definitions(
            Core PRIVATE -DUSE_LIBUNWIND
    )
endif ()

###################################################################################
# 额外的任务配置
###################################################################################
if (SESE_BUILD_TEST AND WIN32)
    target_compile_definitions(Core PUBLIC -DSESE_BUILD_TEST)

    sese_copy_dll_to_binary_path(Core "/sese/test")
    sese_copy_dll_to_binary_path(Core "/sese/benchmark")
endif ()

if (SESE_BUILD_EXAMPLE AND WIN32)
    sese_copy_dll_to_binary_path(Core "/demo")
endif ()

###################################################################################
# Windows 平台配置
###################################################################################
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    file(GLOB_RECURSE NATIVE_WIN_SRC "native/win/*.cpp" "native/win/*.h")
    set(DLL_INFO_RC ${PROJECT_BINARY_DIR}/WindowsDllInfo.rc)
    configure_file(WindowsDllInfo.rc.in ${DLL_INFO_RC})

    target_sources(Core PRIVATE ${DLL_INFO_RC})
    target_sources(Core PRIVATE ${NATIVE_WIN_SRC})
    target_compile_definitions(Core PRIVATE -D_WIN32_WINNT=0x0601)

    if (MSVC)
        target_compile_options(Core PRIVATE "/utf-8")
    endif ()
    target_link_libraries(Core PUBLIC crypt32 ws2_32 wsock32 dbghelp iphlpapi advapi32 secur32)

    # if (${MINGW})
    #     target_compile_options(Core PRIVATE "-fno-exceptions")
    # else ()
    #     target_compile_options(Core PRIVATE "/D_HAS_EXCEPTIONS=0")
    # endif ()
endif ()

###################################################################################
# Linux 平台配置
###################################################################################
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_package(Threads REQUIRED)
    target_link_libraries(Core PRIVATE ${CMAKE_THREAD_LIBS_INIT})
    target_link_libraries(Core PRIVATE ${CMAKE_DL_LIBS})

    file(GLOB_RECURSE NATIVE_LINUX_SRC "native/linux/*.cpp" "native/linux/*.h")
    target_sources(Core PRIVATE ${UNIX_SRC} ${NATIVE_LINUX_SRC})

    # target_compile_options(Core PRIVATE "-fno-exceptions")
    # target_compile_options(Core PUBLIC "-fPIC")
    set_target_properties(Core PROPERTIES POSITION_INDEPENDENT_CODE ON)
    target_link_options(Core PUBLIC "-rdynamic")
endif ()

###################################################################################
# macOS 平台配置
###################################################################################
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    find_library(IOKIT_FRAMEWORK IOKit REQUIRED)
    find_library(CORESERVICES_FRAMEWORK CoreServices REQUIRED)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation REQUIRED)
    target_link_libraries(Core PRIVATE ${COREFOUNDATION_FRAMEWORK} ${IOKIT_FRAMEWORK} ${CORESERVICES_FRAMEWORK})

    # target_compile_options(Core PRIVATE "-fno-exceptions")

    file(GLOB_RECURSE NATIVE_DRAWIN_SRC "native/darwin/*.cpp" "native/darwin/*.h")
    target_sources(Core PRIVATE ${UNIX_SRC} ${NATIVE_DRAWIN_SRC})
endif ()

###################################################################################
# CMake 安装包配置
###################################################################################
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
configure_package_config_file(
        ${PROJECT_SOURCE_DIR}/cmake/sese-config.cmake.in
        ${PROJECT_BINARY_DIR}/sese-config.cmake
        INSTALL_DESTINATION lib/cmake/sese
)

install(
        TARGETS ${ARCHIVE_TARGET} DB Core
        EXPORT SeseTargets
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include
)

install(
        DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp"
        PATTERN "${CMAKE_CURRENT_LIST_DIR}/internal" EXCLUDE
        PATTERN "${CMAKE_CURRENT_LIST_DIR}/test" EXCLUDE
        PATTERN "${CMAKE_CURRENT_LIST_DIR}/example" EXCLUDE
)

install(
        FILES "${PROJECT_BINARY_DIR}/sese-config.cmake"
        DESTINATION lib/cmake/sese
)

install(
        FILES "${PROJECT_BINARY_DIR}/sese-config.cmake"
        DESTINATION debug/lib/cmake/sese
)

install(
        EXPORT SeseTargets
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sese
        NAMESPACE Sese::
)