find_package(GTest CONFIG REQUIRED)

    # mariadb
if (SESE_DB_USE_MARIADB)
    # 添加可执行文件
    add_executable(TestMariaDriver TestMariaDriver.cpp)
    add_executable(TestMariaInstance TestMariaInstance.cpp)
    # 为目标指定源文件
    target_sources(TestMariaDriver PRIVATE ${CMAKE_CURRENT_LIST_DIR}/TestMariaDriver.cpp)
    target_sources(TestMariaInstance PRIVATE ${CMAKE_CURRENT_LIST_DIR}/TestMariaInstance.cpp)
    # 为目标链接库
    target_link_libraries(TestMariaDriver unofficial::libmariadb)
    target_link_libraries(TestMariaInstance DB)
    target_link_libraries(TestMariaDriver GTest::gtest GTest::gtest_main)
    target_link_libraries(TestMariaInstance GTest::gtest GTest::gtest_main)

    if (WIN32)
        target_compile_options(TestMariaInstance PRIVATE /utf-8)
    endif ()
    # 添加测试
    add_test(NAME TestMariaDriver COMMAND $<TARGET_FILE:TestMariaDriver>)
    add_test(NAME TestMariaInstance COMMAND $<TARGET_FILE:TestMariaInstance>)
endif ()

    # sqlite
#if (SESE_DB_USE_SQLITE)
#    add_executable(TestSqliteDriver TestSqliteDriver.cpp)
#    add_executable(TestSqliteInstance TestSqlInstance.cpp)
#
#    target_sources(TestSqliteDriver PRIVATE ${CMAKE_CURRENT_LIST_DIR}/TestSqliteDriver.cpp)
#    target_sources(TestSqliteInstance PRIVATE ${CMAKE_CURRENT_LIST_DIR}/TestSqliteInstance.cpp)
#
#    target_link_libraries(TestSqliteDriver unofficial::sqlite3::sqlite3)
#    target_link_libraries(TestSqliteInstance DB)
#    target_link_libraries(TestSqliteDriver GTest::gtest GTest::gtest_main)
#    target_link_libraries(TestSqliteInstance GTest::gtest GTest::gtest_main)
#    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
#        target_link_libraries(TestSqliteDriver ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})
#    endif ()
#
#    add_test(NAME TestSqliteDriver COMMAND $<TARGET_FIFE:TestSqliteDriver>)
#    add_test(NAME TestSqliteInstance COMMAND $<TARGET_FIFE:TestSqliteInstance>)
#endif ()