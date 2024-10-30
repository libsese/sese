if (SESE_ENABLE_ASAN)
    if (MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fsanitize=address")
        find_path(ASAN_PATH
                NAMES "clang_rt.asan_dynamic-x86_64.dll"
                PATHS "C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/Tools/MSVC"
        )
        if (NOT ASAN_PATH)
            message(WARNING "The dynamic libraries required to enable asan for a particular compiler have not been found, and running them directly may result in errors")
            message(WARNING "See: https://devblogs.microsoft.com/cppblog/msvc-address-sanitizer-one-dll-for-all-runtime-configurations")
        endif ()
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
        file(COPY "${ASAN_PATH}/clang_rt.asan_dynamic-x86_64.dll" DESTINATION ${PROJECT_BINARY_DIR}/bin)
    else ()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -gdwarf-4")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    endif ()
endif ()