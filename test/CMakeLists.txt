find_package(GTest CONFIG REQUIRED)
find_package(Python3 COMPONENTS Interpreter REQUIRED)

set(CMAKE_CXX_STANDARD 20)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/utf-8)
endif ()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")
    set(CMAKE_EXE_LINKER_FLAGS "-Wl,-copy-dt-needed-entries,--export-dynamic")
endif ()

add_definitions("-DPY_EXECUTABLE=\"${Python3_EXECUTABLE}\"")

file(GLOB TEST_SRC "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_executable(
        AllTestInMain
        ${TEST_SRC}
        TestSharedMemory/TestSharedMemory.cpp
        TestPlugin/TestPlugin.cpp
        TestIPC/TestIPC.cpp
)

target_link_libraries(AllTestInMain Core GTest::gtest GTest::gtest_main)
add_test(NAME AllTestInMain COMMAND AllTestInMain)

# TestArchive
if (SESE_USE_ARCHIVE)
    target_sources(
            AllTestInMain
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
target_compile_definitions(AllTestInMain PRIVATE "-DPATH_TO_MODULE=\"$<TARGET_FILE:Module.m>\"")
target_compile_definitions(AllTestInMain PRIVATE "-DPATH_TO_CORE=\"$<TARGET_FILE:Core>\"")
add_dependencies(AllTestInMain Module.m)

# TestSharedMemory
add_executable(Memory.d TestSharedMemory/Memory.d.cpp)
target_link_libraries(Memory.d PRIVATE Core)
set_target_properties(
        Memory.d PROPERTIES
        OUTPUT_NAME "Memory.d"
        PREFIX ""
)
target_compile_definitions(AllTestInMain PRIVATE "-DPATH_TO_MEM_D=\"$<TARGET_FILE:Memory.d>\"")
add_dependencies(AllTestInMain Memory.d)

# TestIPC
add_executable(IPC.d TestIPC/IPC.d.cpp)
target_link_libraries(IPC.d PRIVATE Core)
set_target_properties(
        IPC.d PROPERTIES
        OUTPUT_NAME "IPC.d"
        PREFIX ""
)
target_compile_definitions(AllTestInMain PRIVATE "-DPATH_TO_IPC_D=\"$<TARGET_FILE:IPC.d>\"")
add_dependencies(AllTestInMain IPC.d)

add_subdirectory(TestDB)