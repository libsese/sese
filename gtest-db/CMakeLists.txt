find_package(GTest CONFIG REQUIRED)

if (WIN32)
    add_compile_options(/utf-8)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_EXE_LINKER_FLAGS "-Wl,-copy-dt-needed-entries")
endif ()

if (SESE_GITHUB_WORKFLOW)
    add_definitions(-DSQLITE_CONNECTION_STRING=\"${PROJECT_SOURCE_DIR}/build/db_test.db\")
    add_definitions(-DMYSQL_CONNECTION_STRING=\"host=127.0.0.1;port=3306;user=root;pwd=root;db=db_test;\")
    add_definitions(-DPSQL_CONNECTION_STRING=\"host=127.0.0.1;port=5432;user=postgres;pwd=postgres;db=db_test;\")
endif ()

if (SESE_DB_USE_MARIADB)
    add_executable(TestMaria)
    target_sources(TestMaria PRIVATE
            TestMariaMain.cpp
            TestMariaInstance.cpp
            TestMariaStatement.cpp
            TestMariaTransaction.cpp
            TestMariaMetadata.cpp
    )
    target_link_libraries(TestMaria Core)
    target_link_libraries(TestMaria GTest::gtest GTest::gtest_main)
    add_test(NAME TestMaria COMMAND $<TARGET_FILE:TestMaria>)
endif ()

if (SESE_DB_USE_SQLITE)
    add_executable(TestSqlite)
    target_sources(TestSqlite PRIVATE
            TestSqliteMain.cpp
            TestSqliteInstance.cpp
            TestSqliteStatement.cpp
            TestSqliteTransaction.cpp
            TestSqliteMetadata.cpp
    )
    target_link_libraries(TestSqlite Core)
    target_link_libraries(TestSqlite GTest::gtest GTest::gtest_main)
    target_compile_definitions(TestSqlite PRIVATE -DPATH_TO_DB=\"${PROJECT_SOURCE_DIR}/docker/tmp/db_test.db\")
    add_test(NAME TestSqlite COMMAND $<TARGET_FILE:TestSqlite>)
endif ()

if (SESE_DB_USE_POSTGRES)
    add_executable(TestPostgres)
    target_sources(TestPostgres PRIVATE
            TestPostgresMain.cpp
            TestPostgresInstance.cpp
            TestPostgresStatement.cpp
            TestPostgresTransaction.cpp
            TestPostgresMetadata.cpp
    )
    target_link_libraries(TestPostgres Core)
    target_link_libraries(TestPostgres GTest::gtest GTest::gtest_main)
    add_test(NAME TestPostgres COMMAND $<TARGET_FILE:TestPostgres>)
endif ()