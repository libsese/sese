cmake_minimum_required(VERSION 3.14)
project(sese-db VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS on)
include(FetchContent)

option(SESE_DB_BUILD_TEST "build unit test targets" off)
option(SESE_DB_USE_MARIADB "add mariadb and mysql support" off)
option(SESE_DB_USE_SQLITE "add sqlite support" on)

if (SESE_DB_USE_MARIADB)
    # libmariadb version 3.3
    FetchContent_Declare(mariadb-connector-c
            GIT_REPOSITORY https://github.com/shiinasama/mariadb-connector-c.git
            GIT_TAG a0ae733ca2d68ba4535254ac85a4114611514053
            )
    FetchContent_MakeAvailable(mariadb-connector-c)
endif ()

if (SESE_DB_USE_SQLITE)
    # sqlite version 3.40.0
    FetchContent_Declare(libsqlite
            URL https://www.sqlite.org/2022/sqlite-amalgamation-3400000.zip
            URL_HASH SHA3_256=362a64ad2891c6ff7552f039f3f233d49da984446be0862183eb5cef0d665969
            )
    FetchContent_MakeAvailable(libsqlite)

    add_library(sqlite SHARED)
    target_sources(sqlite PRIVATE
            ${libsqlite_SOURCE_DIR}/shell.c
            ${libsqlite_SOURCE_DIR}/sqlite3.c
            )
    target_include_directories(sqlite PUBLIC ${libsqlite_SOURCE_DIR})
endif ()

if (WIN32)
    add_compile_options(/utf-8)
endif ()
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_package(Threads REQUIRED)
endif ()

add_library(sese-db SHARED)
add_library(sese-db-static)

target_sources(sese-db PRIVATE src/DriverManager.cpp src/Util.cpp)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux" AND SESE_DB_USE_SQLITE)
    target_link_libraries(sese-db ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})
    target_link_libraries(sese-db-static ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})
endif ()

if (SESE_DB_USE_MARIADB)
    target_sources(sese-db PRIVATE
            src/impl/MariaDriverInstanceImpl.cpp
            src/impl/MariaResultSetImpl.cpp
            src/impl/MariaStmtResultSetImpl.cpp
            src/impl/MariaPreparedStatementImpl.cpp)
    target_compile_definitions(sese-db PRIVATE -DHAS_MARIADB)
    target_compile_definitions(sese-db-static PRIVATE -DHAS_MARIADB)
    target_link_libraries(sese-db libmariadb)
    target_link_libraries(sese-db-static libmariadb)
endif ()

if (SESE_DB_USE_SQLITE)
    target_sources(sese-db PRIVATE
            src/impl/SqliteResultSetImpl.cpp
            src/impl/SqlitePreparedStatementImpl.cpp
            src/impl/SqliteStmtResultSetImpl.cpp
            src/impl/SqliteDriverInstanceImpl.cpp)
    target_compile_definitions(sese-db PRIVATE -DHAS_SQLITE)
    target_compile_definitions(sese-db-static PRIVATE -DHAS_SQLITE)
    target_link_libraries(sese-db sqlite)
    target_link_libraries(sese-db-static sqlite)
endif ()

target_include_directories(sese-db PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_include_directories(sese-db-static PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_sources(sese-db-static PRIVATE $<TARGET_OBJECTS:sese-db>)

if (SESE_DB_BUILD_TEST)
    enable_testing()

    if (SESE_DB_USE_MARIADB)
        add_executable(TestMariaDriver)
        target_sources(TestMariaDriver PRIVATE test/TestMariaDriver.cpp)
        target_link_libraries(TestMariaDriver libmariadb)

        add_executable(TestMariaInstance)
        target_sources(TestMariaInstance PRIVATE test/TestMariaInstance.cpp)
        target_link_libraries(TestMariaInstance sese-db)
    endif ()

    if (SESE_DB_USE_SQLITE)
        add_executable(TestSqliteDriver)
        target_sources(TestSqliteDriver PRIVATE test/TestSqliteDriver.cpp)
        target_link_libraries(TestSqliteDriver sqlite)
        if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
            target_link_libraries(TestSqliteDriver ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})
        endif ()

        add_executable(TestSqliteInstance)
        target_sources(TestSqliteInstance PRIVATE test/TestSqliteInstance.cpp)
        target_link_libraries(TestSqliteInstance sese-db)
    endif ()

endif ()