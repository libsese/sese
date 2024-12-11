add_executable(Bundler
    ./Bundler.h
    ./Bundler.cpp
    ./Bundler_RC.cpp
    ./Bundler_ELF.cpp
    ./Bundler_MACH_O.cpp
)

target_link_libraries(Bundler PRIVATE Core)
