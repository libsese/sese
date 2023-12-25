cmake_minimum_required(VERSION 3.14)
project(sese-db VERSION 1.1.2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")

option(SESE_DB_BUILD_TEST "build unit test targets" ON)
option(SESE_DB_USE_MARIADB "add mariadb and mysql support" OFF)
option(SESE_DB_USE_SQLITE "add sqlite support" OFF)
option(SESE_DB_USE_POSTGRES "add postgresql support" OFF)

list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")

message(STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}.")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (UNIX)
        # for valgrind
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -gdwarf-4")
        # for sanitizer
        # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak")
    endif ()
endif ()

add_library(DB SHARED)

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
    target_compile_definitions(DB PRIVATE -DHAS_MARIADB)
endif ()

if (SESE_DB_USE_SQLITE)
    find_package(unofficial-sqlite3 CONFIG REQUIRED)
    target_link_libraries(DB PRIVATE unofficial::sqlite3::sqlite3)
    target_sources(DB PRIVATE
            src/sqlite/SqliteDriverInstanceImpl.cpp
            src/sqlite/SqlitePreparedStatementImpl.cpp
            src/sqlite/SqliteResultSetImpl.cpp
            src/sqlite/SqliteStmtResultSetImpl.cpp
            )
    target_compile_definitions(DB PRIVATE -DHAS_SQLITE)
endif ()

if (SESE_DB_USE_POSTGRES)
    find_package(PostgreSQL REQUIRED)
    target_link_libraries(DB PRIVATE PostgreSQL::PostgreSQL)
    target_sources(DB PRIVATE
            src/pgsql/PostgresDriverInstanceImpl.cpp
            src/pgsql/PostgresResultSetImpl.cpp
            src/pgsql/PostgresPreparedStatementImpl.cpp
            )
    target_compile_definitions(DB PRIVATE -DHAS_POSTGRES)
endif ()

target_include_directories(DB PUBLIC include/impl)
target_include_directories(
        DB
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/interface>
        $<INSTALL_INTERFACE:include/interface>
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
endif()