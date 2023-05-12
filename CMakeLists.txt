cmake_minimum_required(VERSION 3.14)
project(sese.db VERSION 1.1.2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

option(SESE_DB_BUILD_TEST "build unit test targets" ON)
option(SESE_DB_USE_MARIADB "add mariadb and mysql support" ON)
option(SESE_DB_USE_SQLITE "add sqlite support" ON)

add_library(sese-db SHARED)

if (SESE_DB_USE_MARIADB)
    find_package(unofficial-libmariadb CONFIG REQUIRED)
    target_link_libraries(sese-db PRIVATE unofficial::libmariadb)
    target_sources(sese-db PRIVATE
            src/maria/MariaDriverInstanceImpl.cpp
            src/maria/MariaPreparedStatementImpl.cpp
            src/maria/MariaResultSetImpl.cpp
            src/maria/MariaStmtResultSetImpl.cpp
            )
    target_compile_definitions(sese-db PRIVATE -DHAS_MARIADB)
endif ()


if (SESE_DB_USE_SQLITE)
    find_package(unofficial-sqlite3 CONFIG REQUIRED)
    target_link_libraries(sese-db PRIVATE unofficial::sqlite3::sqlite3)
    target_sources(sese-db PRIVATE
            src/sqlite/SqliteDriverInstanceImpl.cpp
            src/sqlite/SqlitePreparedStatementImpl.cpp
            src/sqlite/SqliteResultSetImpl.cpp
            src/sqlite/SqliteStmtResultSetImpl.cpp
            )
    target_compile_definitions(sese-db PRIVATE -DHAS_SQLITE)
endif ()

target_include_directories(sese-db PUBLIC include/impl)
target_include_directories(
        sese-db
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/interface>
        $<INSTALL_INTERFACE:include/interface>
)

if (WIN32)
    target_compile_options(sese-db PRIVATE /utf-8)
else ()
    find_package(Threads REQUIRED)
    target_link_libraries(sese-db ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})
endif ()

target_sources(sese-db PRIVATE src/DriverManager.cpp src/Util.cpp)