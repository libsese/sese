add_executable(EchoServer Server.cpp)
target_link_libraries(EchoServer PRIVATE Core)

if (${MSVC})
    target_compile_options(EchoServer PRIVATE /utf-8)
endif ()