if(SESE_ENABLE_ASAN)
    if(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fsanitize=address")
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64" OR CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64")
            set(ASAN_DLL_NAME "clang_rt.asan_dynamic-x86_64.dll")
        else()
            message(FATAL_ERROR "Asan is used on \"${CMAKE_SYSTEM_PROCESSOR}\" not supported by msvc.")
        endif()
        find_path(
            ASAN_PATH
            NAMES ${ASAN_DLL_NAME}
            PATHS ${VCPKG_VISUAL_STUDIO_PATH}
        )
        if(NOT ASAN_PATH)
            message(
                WARNING
                    "The dynamic libraries required to enable asan for a particular compiler have not been found, and running them directly may result in errors."
            )
            message(
                WARNING
                    "See: https://devblogs.microsoft.com/cppblog/msvc-address-sanitizer-one-dll-for-all-runtime-configurations"
            )
        endif()
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
        file(COPY "${ASAN_PATH}/${ASAN_DLL_NAME}" DESTINATION ${PROJECT_BINARY_DIR}/bin)
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -gdwarf-4")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    endif()
endif()
