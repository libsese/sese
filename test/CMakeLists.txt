find_package(GTest CONFIG REQUIRED)

if (WIN32)
    add_compile_options(/utf-8)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_EXE_LINKER_FLAGS "-Wl,-copy-dt-needed-entries")
endif ()

# mariadb
if (SESE_DB_USE_MARIADB)

    add_executable(TestMariaInstance TestMariaInstance.cpp)

    target_sources(TestMariaInstance PRIVATE ${CMAKE_CURRENT_LIST_DIR}/TestMariaInstance.cpp)

    target_link_libraries(TestMariaInstance DB)

    target_link_libraries(TestMariaInstance GTest::gtest GTest::gtest_main)

    if (WIN32)
        target_compile_options(TestMariaInstance PRIVATE /utf-8)
    endif ()

    # 添加测试
    add_test(NAME TestMariaInstance COMMAND $<TARGET_FILE:TestMariaInstance>)
endif ()

# sqlite
if (SESE_DB_USE_SQLITE)

    add_executable(TestSqliteInstance TestSqliteInstance.cpp)

    target_sources(TestSqliteInstance PRIVATE ${CMAKE_CURRENT_LIST_DIR}/TestSqliteInstance.cpp)

    target_link_libraries(TestSqliteInstance DB)

    target_link_libraries(TestSqliteInstance GTest::gtest GTest::gtest_main)

#    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
#        target_link_libraries(TestSqliteDriver ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})
#    endif ()

    target_compile_definitions(TestSqliteInstance PRIVATE -DPATH_TO_DB=\"${PROJECT_SOURCE_DIR}/docker/tmp/db_test.db\")

    add_test(NAME TestSqliteInstance COMMAND $<TARGET_FILE:TestSqliteInstance>)
endif ()

# postgresql
if (SESE_DB_USE_POSTGRES)

    add_executable(TestPostgresInstance TestPostgresInstance.cpp)

    target_sources(TestPostgresInstance PRIVATE ${CMAKE_CURRENT_LIST_DIR}/TestPostgresInstance.cpp)

    target_link_libraries(TestPostgresInstance DB)

    target_link_libraries(TestPostgresInstance GTest::gtest GTest::gtest_main)

    if (WIN32)
        target_compile_options(TestPostgresInstance PRIVATE /utf-8)
    endif ()

    add_test(NAME TestPostgresInstance COMMAND $<TARGET_FILE:TestPostgresInstance>)
endif ()