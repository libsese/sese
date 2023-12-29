cmake_minimum_required(VERSION 3.14)

cmake_policy(SET CMP0048 NEW)

macro(auto_enable_feature opt_name feature_name)
    if (${opt_name})
        if (NOT DEFINED VCPKG_MANIFEST_FEATURES)
            message(STATUS "Auto append features: ${feature_name}")
            set(VCPKG_MANIFEST_FEATURES ${feature_name})
        else ()
            list(FIND VCPKG_MANIFEST_FEATURES ${opt_name} index)
            if (index EQUAL -1)
                message(STATUS "Auto append features: ${feature_name}")
                list(APPEND VCPKG_MANIFEST_FEATURES ${feature_name})
            endif ()
        endif ()
    endif ()
endmacro()

option(SESE_DB_BUILD_TEST "build unit test targets" OFF)
option(SESE_DB_USE_MARIADB "add mariadb and mysql support" OFF)
option(SESE_DB_USE_SQLITE "add sqlite support" OFF)
option(SESE_DB_USE_POSTGRES "add postgresql support" OFF)

auto_enable_feature(SESE_DB_BUILD_TEST "tests")
auto_enable_feature(SESE_DB_USE_MARIADB "mysql")
auto_enable_feature(SESE_DB_USE_SQLITE "sqlite3")
auto_enable_feature(SESE_DB_USE_POSTGRES "psql")

project(sese-db VERSION 1.1.3)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (UNIX)
        # for valgrind
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -gdwarf-4")
        # for sanitizer
        # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    endif ()
endif ()

add_library(DB)

if (${CMAKE_GENERATOR} MATCHES "Ninja")
    # 单配置
    add_custom_command(
            TARGET
            DB
            POST_BUILD
            COMMAND
            ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:DB> ${PROJECT_BINARY_DIR}/test
    )
else ()
    # 多配置
    add_custom_command(
            TARGET
            DB
            POST_BUILD
            COMMAND
            ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:DB> ${PROJECT_BINARY_DIR}/test/$<CONFIG>
    )
endif ()

if (SESE_DB_USE_MARIADB)
    find_package(unofficial-libmariadb CONFIG REQUIRED)
    target_link_libraries(DB PRIVATE unofficial::libmariadb)
    target_sources(DB PRIVATE
            src/maria/MariaDriverInstanceImpl.cpp
            src/maria/MariaPreparedStatementImpl.cpp
            src/maria/MariaResultSetImpl.cpp
            src/maria/MariaStmtResultSetImpl.cpp
    )
    add_definitions(-DHAS_MARIADB)
endif ()

find_package(sese CONFIG REQUIRED)
target_link_libraries(DB PUBLIC Sese::Core)

if (SESE_DB_USE_SQLITE)
    find_package(unofficial-sqlite3 CONFIG REQUIRED)
    target_link_libraries(DB PRIVATE unofficial::sqlite3::sqlite3)
    target_sources(DB PRIVATE
            src/sqlite/SqliteDriverInstanceImpl.cpp
            src/sqlite/SqlitePreparedStatementImpl.cpp
            src/sqlite/SqliteResultSetImpl.cpp
            src/sqlite/SqliteStmtResultSetImpl.cpp
    )
    add_definitions(-DHAS_SQLITE)
endif ()

if (SESE_DB_USE_POSTGRES)
    find_package(PostgreSQL REQUIRED)
    target_link_libraries(DB PRIVATE PostgreSQL::PostgreSQL)
    target_sources(DB PRIVATE
            src/pgsql/PostgresDriverInstanceImpl.cpp
            src/pgsql/PostgresResultSetImpl.cpp
            src/pgsql/PostgresPreparedStatementImpl.cpp
    )
    add_definitions(-DHAS_POSTGRES)
endif ()

target_include_directories(
        DB
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/impl>
        $<INSTALL_INTERFACE:include>
)
target_include_directories(
        DB
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/interface>
        $<INSTALL_INTERFACE:include>
)

if (WIN32)
    target_compile_options(DB PRIVATE /utf-8)
else ()
    find_package(Threads REQUIRED)
    target_link_libraries(DB PRIVATE ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})
endif ()

target_sources(DB PRIVATE src/DriverManager.cpp src/Util.cpp)

set_target_properties(
        DB
        PROPERTIES
        OUTPUT_NAME "sese.db"
        PREFIX ""
)

if (SESE_DB_BUILD_TEST)
    enable_testing()
    add_subdirectory(test)
endif ()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
configure_package_config_file(
        ${PROJECT_SOURCE_DIR}/cmake/sese-db-config.cmake.in
        ${PROJECT_BINARY_DIR}/sese-db-config.cmake
        INSTALL_DESTINATION lib/cmake/sese-db
)

install(
        TARGETS DB
        EXPORT SeseDbTargets
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include
)

install(
        DIRECTORY "${PROJECT_SOURCE_DIR}/include/interface/sese"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

install(
        FILES "${PROJECT_BINARY_DIR}/sese-db-config.cmake"
        DESTINATION lib/cmake/sese-db
)

install(
        FILES "${PROJECT_BINARY_DIR}/sese-db-config.cmake"
        DESTINATION debug/lib/cmake/sese-db
)

install(
        EXPORT SeseDbTargets
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sese-db
        NAMESPACE Sese::
)