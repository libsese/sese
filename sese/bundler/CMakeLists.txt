add_executable(Bundler
    ./Bundler.h
    ./Bundler.cpp
    ./Bundler_RC.cpp
    ./Bundler_LD.cpp
)

target_link_libraries(Bundler PRIVATE Core)
