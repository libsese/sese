add_executable(EchoServer EchoServer.cpp)
target_link_libraries(EchoServer PRIVATE Core)

add_executable(HttpServer HttpServer.cpp)
target_link_libraries(HttpServer PRIVATE Core)

add_executable(Json2yml Json2yml.cpp)
target_link_libraries(Json2yml PRIVATE Core)

if (${MSVC})
    target_compile_options(HttpServer PRIVATE /utf-8)
    target_compile_options(EchoServer PRIVATE /utf-8)
    target_compile_options(Json2yml PRIVATE /utf-8)
endif ()