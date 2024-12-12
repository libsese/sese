set(CMAKE_ENABLE_EXPORTS ON)
if (MSVC)
    add_compile_options(/utf-8)
endif ()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")
    set(CMAKE_EXE_LINKER_FLAGS "-Wl,-copy-dt-needed-entries,--export-dynamic")
endif ()

add_definitions("-DPY_EXECUTABLE=\"${Python3_EXECUTABLE}\"")
add_definitions("-DPROJECT_PATH=\"${PROJECT_SOURCE_DIR}\"")
add_definitions("-DPROJECT_BINARY_PATH=\"${CMAKE_CURRENT_BINARY_DIR}\"")

file(GLOB TEST_SRC "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_executable(
        AllTestsInMain
        ${TEST_SRC}
        TestSharedMemory/TestSharedMemory.cpp
        TestPlugin/TestPlugin.cpp
        TestIPC/TestIPC.cpp
        TestResource/TestResource.cpp
)

target_link_libraries(AllTestsInMain Core GTest::gtest GTest::gtest_main)
add_test(NAME AllTestsInMain COMMAND AllTestsInMain)

if (MSVC AND SESE_ENABLE_ASAN)
    set_target_properties(AllTestsInMain PROPERTIES
            INSTALL_RPATH ${ASAN_PATH}
    )
endif ()

# TestArchive
if (SESE_USE_ARCHIVE)
    target_sources(
            AllTestsInMain
            PRIVATE
            TestArchive/TestArchiveReader.cpp
            TestArchive/TestArchiveWriter.cpp
    )
endif ()

# TestPlugin
add_library(Module.m SHARED TestPlugin/Module.m.cpp)
target_link_libraries(Module.m PUBLIC Core)
set_target_properties(
        Module.m PROPERTIES
        OUTPUT_NAME "Module.m"
        PREFIX ""
        SUFFIX ""
)
target_compile_definitions(AllTestsInMain PRIVATE "-DPATH_TO_MODULE=\"$<TARGET_FILE:Module.m>\"")
target_compile_definitions(AllTestsInMain PRIVATE "-DPATH_TO_CORE=\"$<TARGET_FILE:Core>\"")
add_dependencies(AllTestsInMain Module.m)

# TestResource
include(${PROJECT_SOURCE_DIR}/cmake/Bundler.cmake)
target_include_resource(AllTestsInMain TestResource TestResource/Resource.json)


# TestSharedMemory
add_executable(Memory.d TestSharedMemory/Memory.d.cpp)
target_link_libraries(Memory.d PRIVATE Core)
set_target_properties(
        Memory.d PROPERTIES
        OUTPUT_NAME "Memory.d"
        PREFIX ""
)
target_compile_definitions(AllTestsInMain PRIVATE "-DPATH_TO_MEM_D=\"$<TARGET_FILE:Memory.d>\"")
add_dependencies(AllTestsInMain Memory.d)

# TestIPC
add_executable(IPC.d TestIPC/IPC.d.cpp)
target_link_libraries(IPC.d PRIVATE Core)
set_target_properties(
        IPC.d PROPERTIES
        OUTPUT_NAME "IPC.d"
        PREFIX ""
)
target_compile_definitions(AllTestsInMain PRIVATE "-DPATH_TO_IPC_D=\"$<TARGET_FILE:IPC.d>\"")
add_dependencies(AllTestsInMain IPC.d)

# TestSqlite
if (SESE_DB_USE_SQLITE)
    target_sources(AllTestsInMain PRIVATE
            ./TestDB/TestSqliteInstance.cpp
            ./TestDB/TestSqliteStatement.cpp
            ./TestDB/TestSqliteTransaction.cpp
            ./TestDB/TestSqliteMetadata.cpp
    )
    if (DEFINED SESE_DB_SQLITE_CONNECTION_STRING)
        target_compile_definitions(
                AllTestsInMain PRIVATE
                "SQLITE_CONNECTION_STRING=\"${SESE_DB_SQLITE_CONNECTION_STRING}\""
        )
    else ()
        target_compile_definitions(
                AllTestsInMain PRIVATE
                "PATH_TO_DB=\"${PROJECT_SOURCE_DIR}/build/db_test.db\""
        )
    endif ()
endif ()

# TestMaria
if (SESE_DB_USE_MARIADB)
    target_sources(AllTestsInMain PRIVATE
            ./TestDB/TestMariaInstance.cpp
            ./TestDB/TestMariaStatement.cpp
            ./TestDB/TestMariaTransaction.cpp
            ./TestDB/TestMariaMetadata.cpp
    )
    if (DEFINED SESE_DB_MYSQL_CONNECTION_STRING)
        target_compile_definitions(
                AllTestsInMain PRIVATE
                "MYSQL_CONNECTION_STRING=\"${SESE_DB_MYSQL_CONNECTION_STRING}\""
        )
    endif ()
endif ()

# TestPostgres
if (SESE_DB_USE_POSTGRES)
    target_sources(AllTestsInMain PRIVATE
            ./TestDB/TestPostgresInstance.cpp
            ./TestDB/TestPostgresStatement.cpp
            ./TestDB/TestPostgresTransaction.cpp
            ./TestDB/TestPostgresMetadata.cpp
    )
    if (DEFINED SESE_DB_PSQL_CONNECTION_STRING)
        target_compile_definitions(
                AllTestsInMain PRIVATE
                "PSQL_CONNECTION_STRING=\"${SESE_DB_PSQL_CONNECTION_STRING}\""
        )
    endif ()
endif ()