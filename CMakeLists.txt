cmake_minimum_required(VERSION 3.14)
project(sese-db)

include(FetchContent)

option(BUILD_TEST "build unit test targets" off)
option(USE_MARIADB "add mariadb any mysql support" off)
option(USE_SQLITE "add sqlite support" on)

if (USE_MARIADB)
    # libmariadb version 3.3
    FetchContent_Declare(mariadb-connector-c
            GIT_REPOSITORY https://github.com/shiinasama/mariadb-connector-c.git
            GIT_TAG a0ae733ca2d68ba4535254ac85a4114611514053
            )
    FetchContent_MakeAvailable(mariadb-connector-c)
endif ()

if (USE_SQLITE)
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

if (BUILD_TEST)
    enable_testing()

    if (USE_MARIADB)
        add_executable(TestMariaDriver)
        target_sources(TestMariaDriver PRIVATE test/TestMariaDriver.cpp)
        target_link_libraries(TestMariaDriver libmariadb)
    endif ()

    if (USE_SQLITE)
        add_executable(TestSqliteDriver)
        target_sources(TestSqliteDriver PRIVATE test/TestSqliteDriver.cpp)
        target_link_libraries(TestSqliteDriver sqlite)
    endif ()

endif ()