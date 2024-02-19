set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(LibArchive_INCLUDE_DIR "/usr/local/opt/libarchive/include")
endif ()

find_package(LibArchive REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

add_library(Plugin STATIC
        plugin/BaseClass.h
        plugin/ClassFactory.cpp
        plugin/ClassFactory.h
        plugin/Marco.h
        plugin/ModuleInfo.h
)

file(GLOB_RECURSE ARCHIVE_SRC "${CMAKE_CURRENT_LIST_DIR}/archive/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/archive/*.h")
file(GLOB_RECURSE CONCURRENT_SRC "${CMAKE_CURRENT_LIST_DIR}/concurrent/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/concurrent/*.h")
file(GLOB_RECURSE DB_SRC "${CMAKE_CURRENT_LIST_DIR}/db/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/db/*.h")
file(GLOB_RECURSE EVENT_SRC "${CMAKE_CURRENT_LIST_DIR}/event/*.h")
file(GLOB_RECURSE RECORD_SRC "${CMAKE_CURRENT_LIST_DIR}/record/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/record/*.h")
file(GLOB_RECURSE CONVERT_SRC "${CMAKE_CURRENT_LIST_DIR}/convert/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/convert/*.h")
file(GLOB_RECURSE IO_SRC "${CMAKE_CURRENT_LIST_DIR}/io/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/io/*.h")
file(GLOB_RECURSE THREAD_SRC "${CMAKE_CURRENT_LIST_DIR}/thread/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/thread/*.h")
file(GLOB_RECURSE SYSTEM_SRC "${CMAKE_CURRENT_LIST_DIR}/system/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/system/*.h")
file(GLOB_RECURSE NET_SRC "${CMAKE_CURRENT_LIST_DIR}/net/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/net/*.h")
file(GLOB_RECURSE CONFIG_SRC "${CMAKE_CURRENT_LIST_DIR}/config/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/config/*.h")
file(GLOB_RECURSE TEXT_SRC "${CMAKE_CURRENT_LIST_DIR}/text/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/text/*.h")
file(GLOB_RECURSE UTIL_SRC "${CMAKE_CURRENT_LIST_DIR}/util/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/util/*.h")
file(GLOB_RECURSE SECURITY_SRC "${CMAKE_CURRENT_LIST_DIR}/security/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/security/*.h")
file(GLOB_RECURSE PLUGIN_SRC "${CMAKE_CURRENT_LIST_DIR}/plugin/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/plugin/*.h")
file(GLOB_RECURSE SERVICE_SRC "${CMAKE_CURRENT_LIST_DIR}/service/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/service/*.h")
file(GLOB_RECURSE UNIX_SRC "${CMAKE_CURRENT_LIST_DIR}/native/unix/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/native/unix/*.h")
file(GLOB_RECURSE RES_SRC "${CMAKE_CURRENT_LIST_DIR}/res/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/res/*.h")

add_library(Core
        ${ARCHIVE_SRC}
        ${CONCURRENT_SRC}
        ${CONVERT_SRC}
        ${DB_SRC}
        ${EVENT_SRC}
        ${RECORD_SRC}
        ${INTERNAL_SRC}
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
        ${RES_SRC}
        Config.h
)

if (SESE_DB_USE_SQLITE)
    add_definitions(-DHAS_SQLITE)
    file(
            GLOB_RECURSE
            SQLITE_SRC
            "${CMAKE_CURRENT_LIST_DIR}/internal/db/impl/sqlite/*.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/internal/db/impl/sqlite/*.h"
    )
    target_sources(Core PRIVATE ${SQLITE_SRC})

    if (SESE_USE_NATIVE_MANAGER)
        find_package(SQLite3 REQUIRED)
        target_link_libraries(Core PRIVATE SQLite::SQLite3)
    else ()
        find_package(unofficial-sqlite3 CONFIG REQUIRED)
        target_link_libraries(Core PRIVATE unofficial::sqlite3::sqlite3)
    endif ()
endif ()

if (SESE_DB_USE_MARIADB)
    add_definitions(-DHAS_MARIADB)
    file(
            GLOB_RECURSE
            MARIA_SRC
            "${CMAKE_CURRENT_LIST_DIR}/internal/db/impl/maria/*.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/internal/db/impl/maria/*.h"
    )

    target_sources(Core PRIVATE ${MARIA_SRC})
    if (SESE_USE_NATIVE_MANAGER)
        if (CMAKE_SYSTEM_NAME MATCHES "Linux")
            find_package(MySQL MODULE REQUIRED)
            target_link_libraries(Core PRIVATE ${MYSQL_LIB})
        elseif (CMAKE_SYSTEM_NAME MATCHES "Darwin")
            find_path(MYSQL_INCLUDE_DIR mysql.h
                    /usr/local/include/mysql
                    /usr/local/mysql/include
                    /usr/include/mysql
            )
            find_library(
                    MYSQL_LIB
                    NAME mysqlclient
                    PATHS /usr/lib /usr/local/lib /usr/local/mysql/lib
                    PATH_SUFFIXES mysql
            )

            message(STATUS "MYSQL_INCLUDE_DIR :${MYSQL_INCLUDE_DIR}")
            message(STATUS "MYSQL_LIB         :${MYSQL_LIB}")
            if (MYSQL_INCLUDE_DIR AND MYSQL_LIB)
                target_include_directories(Core PRIVATE ${MYSQL_INCLUDE_DIR})
                target_link_libraries(Core PRIVATE ${MYSQL_LIB})
            else ()
                message(FATAL_ERROR "Could NOT find MySQL library")
            endif ()
        endif ()
    else ()
        find_package(unofficial-libmariadb CONFIG REQUIRED)
        target_link_libraries(Core PRIVATE unofficial::libmariadb)
    endif ()
endif ()

if (SESE_DB_USE_POSTGRES)
    add_definitions(-DHAS_POSTGRES)
    file(
            GLOB_RECURSE
            PSQL_SRC
            "${CMAKE_CURRENT_LIST_DIR}/internal/db/impl/pgsql/*.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/internal/db/impl/pgsql/*.h"
    )
    target_sources(Core PRIVATE ${PSQL_SRC})
    find_package(PostgreSQL REQUIRED)
    target_link_libraries(Core PRIVATE PostgreSQL::PostgreSQL)
endif ()

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
    file(GLOB_RECURSE NATIVE_WIN_SRC "${CMAKE_CURRENT_LIST_DIR}/native/win/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/native/win/*.h")
    set(DLL_INFO_RC ${PROJECT_BINARY_DIR}/WindowsDllInfo.rc)
    configure_file(${PROJECT_SOURCE_DIR}/WindowsDllInfo.rc.in ${DLL_INFO_RC})

    target_sources(Core PRIVATE ${DLL_INFO_RC})
    target_sources(Core PRIVATE ${NATIVE_WIN_SRC})

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
endif ()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_package(Threads REQUIRED)

    file(GLOB_RECURSE NATIVE_LINUX_SRC "${CMAKE_CURRENT_LIST_DIR}/native/linux/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/native/linux/*.h")
    target_sources(Core PRIVATE ${UNIX_SRC} ${NATIVE_LINUX_SRC})

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

    file(GLOB_RECURSE NATIVE_DRAWIN_SRC "${CMAKE_CURRENT_LIST_DIR}/native/darwin/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/native/darwin/*.h")
    target_sources(Core PRIVATE ${UNIX_SRC} ${NATIVE_DRAWIN_SRC})

    target_compile_options(Core PRIVATE "-fno-exceptions")
    target_link_libraries(Core PRIVATE ${COREFOUNDATION_FRAMEWORK} ${IOKIT_FRAMEWORK} ${CORESERVICES_FRAMEWORK})
endif ()

set_target_properties(
        Core PROPERTIES
        CXX_STANDARD 17
        OUTPUT_NAME "sese.core"
        PREFIX ""
        WINDOWS_EXPORT_ALL_SYMBOLS ON
)

set_target_properties(
        Plugin PROPERTIES
        CXX_STANDARD 17
)

target_compile_definitions(Core PRIVATE "SESE_MAJOR_VERSION=\"${PROJECT_VERSION_MAJOR}\"")
target_compile_definitions(Core PRIVATE "SESE_MINOR_VERSION=\"${PROJECT_VERSION_MINOR}\"")
target_compile_definitions(Core PRIVATE "SESE_PATCH_VERSION=\"${PROJECT_VERSION_PATCH}\"")
target_compile_definitions(Core PRIVATE "SESE_REPO_HASH=\"${SESE_REPO_HASH}\"")
target_compile_definitions(Core PRIVATE "SESE_REPO_BRANCH=\"${SESE_REPO_BRANCH}\"")

target_link_libraries(Core PUBLIC Plugin)
target_link_libraries(Core PRIVATE LibArchive::LibArchive)
target_link_libraries(Core PRIVATE OpenSSL::Crypto OpenSSL::SSL)
target_link_libraries(Core PRIVATE ZLIB::ZLIB)

if (DEFINED SESE_RECORD_TIME_PATTERN)
    target_compile_definitions(
            Core PRIVATE
            "SESE_RECORD_TIME_PATTERN=${SESE_RECORD_TIME_PATTERN}"
    )
endif ()

if (DEFINED SESE_RECORD_TEXT_PATTERN)
    target_compile_definitions(
            Core PRIVATE
            "SESE_RECORD_TEXT_PATTERN=${SESE_RECORD_TEXT_PATTERN}"
    )
endif ()

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
        DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp"
        PATTERN "${CMAKE_CURRENT_LIST_DIR}/internal" EXCLUDE
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