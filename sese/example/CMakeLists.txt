if(${MSVC})
    add_compile_options(/utf-8)
endif()

include(${PROJECT_SOURCE_DIR}/cmake/Manifest.cmake)

add_definitions("-DPROJECT_PATH=\"${PROJECT_SOURCE_DIR}\"")
add_definitions("-DPROJECT_BINARY_PATH=\"${CMAKE_CURRENT_BINARY_DIR}\"")

add_executable(EchoServer EchoServer.cpp)
target_link_libraries(EchoServer PRIVATE Core)
target_manifest(EchoServer manifest.json)

add_executable(HttpServer HttpServer.cpp)
target_link_libraries(HttpServer PRIVATE Core)
target_manifest(HttpServer manifest.json)

add_executable(Json2yml Json2yml.cpp)
target_link_libraries(Json2yml PRIVATE Core)
target_manifest(Json2yml manifest.json)
