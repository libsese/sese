if(${MSVC})
    add_compile_options(/utf-8)
endif()

add_definitions("-DPROJECT_PATH=\"${PROJECT_SOURCE_DIR}\"")
add_definitions("-DPROJECT_BINARY_PATH=\"${CMAKE_CURRENT_BINARY_DIR}\"")

add_executable(EchoServer EchoServer.cpp)
target_link_libraries(EchoServer PRIVATE Core)

add_executable(HttpServer HttpServer.cpp)
target_link_libraries(HttpServer PRIVATE Core)

add_executable(Json2yml Json2yml.cpp)
target_link_libraries(Json2yml PRIVATE Core)
