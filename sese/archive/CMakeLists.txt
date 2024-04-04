add_library(Archive OBJECT
        ./ArchiveInputStream.cpp
        ./ArchiveInputStream.h
        ./ArchiveOutputStream.cpp
        ./ArchiveOutputStream.h
        ./ArchiveReader.cpp
        ./ArchiveReader.h
        ./ArchiveWriter.cpp
        ./ArchiveWriter.h
        ./Config.cpp
        ./Config.h
)

if (UNIX)
    set_target_properties(Archive PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(LibArchive_INCLUDE_DIR "/usr/local/opt/libarchive/include")
endif ()
find_package(LibArchive REQUIRED)
target_link_libraries(Archive PRIVATE LibArchive::LibArchive)