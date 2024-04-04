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

if (SESE_DB_USE_SQLITE)
    target_compile_definitions(DB PUBLIC -DHAS_SQLITE)
    target_sources(DB PRIVATE
            ../internal/db/impl/sqlite/SqliteDriverInstanceImpl.cpp
            ../internal/db/impl/sqlite/SqliteDriverInstanceImpl.h
            ../internal/db/impl/sqlite/SqlitePreparedStatementImpl.cpp
            ../internal/db/impl/sqlite/SqlitePreparedStatementImpl.h
            ../internal/db/impl/sqlite/SqliteResultSetImpl.cpp
            ../internal/db/impl/sqlite/SqliteResultSetImpl.h
            ../internal/db/impl/sqlite/SqliteStmtResultSetImpl.cpp
            ../internal/db/impl/sqlite/SqliteStmtResultSetImpl.h
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
            ../internal/db/impl/maria/MariaDriverInstanceImpl.cpp
            ../internal/db/impl/maria/MariaDriverInstanceImpl.h
            ../internal/db/impl/maria/MariaPreparedStatementImpl.cpp
            ../internal/db/impl/maria/MariaPreparedStatementImpl.h
            ../internal/db/impl/maria/MariaResultSetImpl.cpp
            ../internal/db/impl/maria/MariaResultSetImpl.h
            ../internal/db/impl/maria/MariaStmtResultSetImpl.cpp
            ../internal/db/impl/maria/MariaStmtResultSetImpl.h
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
            ../internal/db/impl/pgsql/PostgresDriverInstanceImpl.cpp
            ../internal/db/impl/pgsql/PostgresDriverInstanceImpl.h
            ../internal/db/impl/pgsql/PostgresPreparedStatementImpl.cpp
            ../internal/db/impl/pgsql/PostgresPreparedStatementImpl.h
            ../internal/db/impl/pgsql/PostgresResultSetImpl.cpp
            ../internal/db/impl/pgsql/PostgresResultSetImpl.h
    )
    find_package(PostgreSQL REQUIRED)
    target_link_libraries(DB PUBLIC PostgreSQL::PostgreSQL)
endif ()