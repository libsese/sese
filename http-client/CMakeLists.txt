add_executable(HttpClient Client.cpp)
target_link_libraries(HttpClient PRIVATE Core)

if (${MSVC})
    target_compile_options(HttpClient PRIVATE /utf-8)
endif ()