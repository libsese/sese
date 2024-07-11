add_library(DB OBJECT
        ./Config.h
        ./DriverInstance.h
        ./DriverManager.cpp
        ./DriverManager.h
        ./Metadata.h
        ./PreparedStatement.h
        ./ResultSet.h
        ./Util.cpp
        ./Util.h
)

if (UNIX)
    set_target_properties(DB PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif ()

if (SESE_DB_USE_SQLITE)
    target_compile_definitions(DB PUBLIC -DHAS_SQLITE)
    target_sources(DB PRIVATE
            ../internal/db/sqlite/SqliteDriverInstanceImpl.cpp
            ../internal/db/sqlite/SqliteDriverInstanceImpl.h
            ../internal/db/sqlite/SqlitePreparedStatementImpl.cpp
            ../internal/db/sqlite/SqlitePreparedStatementImpl.h
            ../internal/db/sqlite/SqliteResultSetImpl.cpp
            ../internal/db/sqlite/SqliteResultSetImpl.h
            ../internal/db/sqlite/SqliteStmtResultSetImpl.cpp
            ../internal/db/sqlite/SqliteStmtResultSetImpl.h
    )
    if (SESE_USE_NATIVE_MANAGER)
        find_package(SQLite3 REQUIRED)
        target_link_libraries(DB PUBLIC SQLite::SQLite3)
    else ()
        find_package(unofficial-sqlite3 CONFIG REQUIRED)
        target_link_libraries(DB PUBLIC unofficial::sqlite3::sqlite3)
    endif ()
endif ()

if (SESE_DB_USE_MARIADB)
    target_compile_definitions(DB PUBLIC -DHAS_MARIADB)
    target_sources(DB PRIVATE
            ../internal/db/maria/MariaDriverInstanceImpl.cpp
            ../internal/db/maria/MariaDriverInstanceImpl.h
            ../internal/db/maria/MariaPreparedStatementImpl.cpp
            ../internal/db/maria/MariaPreparedStatementImpl.h
            ../internal/db/maria/MariaResultSetImpl.cpp
            ../internal/db/maria/MariaResultSetImpl.h
            ../internal/db/maria/MariaStmtResultSetImpl.cpp
            ../internal/db/maria/MariaStmtResultSetImpl.h
    )
    if (SESE_USE_NATIVE_MANAGER)
        if (CMAKE_SYSTEM_NAME MATCHES "Linux")
            find_package(MySQL MODULE REQUIRED)
            target_link_libraries(DB PRIVATE ${MYSQL_LIB})
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
                target_include_directories(DB PUBLIC ${MYSQL_INCLUDE_DIR})
                target_link_libraries(DB PUBLIC ${MYSQL_LIB})
            else ()
                message(FATAL_ERROR "Could NOT find MySQL library")
            endif ()
        endif ()
    else ()
        find_package(unofficial-libmariadb CONFIG REQUIRED)
        target_link_libraries(DB PUBLIC unofficial::libmariadb)
    endif ()
endif ()

if (SESE_DB_USE_POSTGRES)
    target_compile_definitions(DB PUBLIC -DHAS_POSTGRES)
    target_sources(DB PRIVATE
            ../internal/db/pgsql/PostgresDriverInstanceImpl.cpp
            ../internal/db/pgsql/PostgresDriverInstanceImpl.h
            ../internal/db/pgsql/PostgresPreparedStatementImpl.cpp
            ../internal/db/pgsql/PostgresPreparedStatementImpl.h
            ../internal/db/pgsql/PostgresResultSetImpl.cpp
            ../internal/db/pgsql/PostgresResultSetImpl.h
    )
    find_package(PostgreSQL REQUIRED)
    target_link_libraries(DB PUBLIC PostgreSQL::PostgreSQL)
endif ()