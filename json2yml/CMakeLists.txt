add_executable(json2yml json2yml.cpp)
target_link_libraries(json2yml PRIVATE Core)

if (${MSVC})
    target_compile_options(json2yml PRIVATE /utf-8)
endif ()