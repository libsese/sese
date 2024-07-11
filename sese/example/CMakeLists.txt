if (${MSVC})
    add_compile_options(/utf-8)
endif ()

add_executable(EchoServer EchoServer.cpp)
target_link_libraries(EchoServer PRIVATE Core)

add_executable(HttpServer HttpServer.cpp)
target_link_libraries(HttpServer PRIVATE Core)

add_executable(Json2yml Json2yml.cpp)
target_link_libraries(Json2yml PRIVATE Core)