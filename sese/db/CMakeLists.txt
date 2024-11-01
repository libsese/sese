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
    target_link_libraries(DB PUBLIC unofficial::sqlite3::sqlite3)
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
    target_link_libraries(DB PUBLIC unofficial::libmariadb)
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
    target_link_libraries(DB PUBLIC PostgreSQL::PostgreSQL)
endif ()