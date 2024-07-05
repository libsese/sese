find_package(benchmark CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

if (MSVC)
    add_compile_options(/utf-8)
endif ()

macro (bm_link_libraries target)
    target_link_libraries(${target} PRIVATE
            benchmark::benchmark
            GTest::gtest
            Core
    )
endmacro ()

add_executable(BM_StringBuilder BM_StringBuilder.cpp)
bm_link_libraries(BM_StringBuilder)

add_executable(BM_PostgresPreparedStatement BM_PostgresPreparedStatement.cpp)
bm_link_libraries(BM_PostgresPreparedStatement)

add_executable(BM_FormatParse BM_FormatParse.cpp)
bm_link_libraries(BM_FormatParse)
