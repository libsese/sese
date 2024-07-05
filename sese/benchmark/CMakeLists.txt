find_package(benchmark CONFIG REQUIRED)

if (MSVC)
    add_compile_options(/utf-8)
endif ()

add_executable(BM_StringBuilder BM_StringBuilder.cpp)
target_link_libraries(BM_StringBuilder PRIVATE
        benchmark::benchmark
        Core
)

add_executable(BM_PostgresPreparedStatement BM_PostgresPreparedStatement.cpp)
target_link_libraries(BM_PostgresPreparedStatement PRIVATE
        benchmark::benchmark
        Core
)