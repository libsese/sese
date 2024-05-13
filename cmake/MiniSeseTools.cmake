macro(sese_auto_enable_feature opt_name feature_name)
    if (${opt_name})
        if (NOT DEFINED VCPKG_MANIFEST_FEATURES)
            message(STATUS "Auto append features: ${feature_name}")
            set(VCPKG_MANIFEST_FEATURES ${feature_name})
        else ()
            list(FIND VCPKG_MANIFEST_FEATURES ${opt_name} index)
            if (index EQUAL -1)
                message(STATUS "Auto append features: ${feature_name}")
                list(APPEND VCPKG_MANIFEST_FEATURES ${feature_name})
            endif ()
        endif ()
    endif ()
endmacro()

macro (sese_proxy ADDR)
    set($ENV{http_proxy} ${ADDR})
    set($ENV{https_proxy} ${ADDR})
    set($ENV{all_proxy} ${ADDR})
endmacro ()

macro (sese_clean_proxy)
    set($ENV{http_proxy} "")
    set($ENV{https_proxy} "")
    set($ENV{all_proxy} "")
endmacro ()

macro(sese_auto_detect_triplet)
    if (WIN32)
        set (VCPKG_TARGET_TRIPLET x64-windows)
    elseif (UNIX AND NOT APPLE)
        set (VCPKG_TARGET_TRIPLET x64-linux-dynamic)
    elseif (APPLE)
        set (VCPKG_TARGET_TRIPLET x64-osx-dynamic)
    else ()
        message (FATAL_ERROR "Unknown systems")
    endif ()
endmacro()

macro(sese_auto_find_vcpkg)
    if (NOT VCPKG_FOUND)
        if (DEFINED CMAKE_TOOLCHAIN_FILE)
            get_filename_component(FILENAME ${CMAKE_TOOLCHAIN_FILE} NAME)
            if (${FILENAME} STREQUAL "vcpkg.cmake")
                set(VCPKG_FOUND ON)
            endif ()
        endif ()

        if (NOT VCPKG_FOUND)
            if (NOT $ENV{VCPKG_ROOT} STREQUAL "")
                set(VCPKG_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
                message(STATUS "VCPKG_TOOLCHAIN_FILE: ${VCPKG_TOOLCHAIN_FILE}")
                include(${VCPKG_TOOLCHAIN_FILE})
            else ()
                find_path(VCPKG_TOOLCHAIN_DIR vcpkg.cmake
                        C:/src/vcpkg/scripts/buildsystems
                        C:/vcpkg/scripts/buildsystems
                        D:/vcpkg/scripts/buildsystems
                        E:/vcpkg/scripts/buildsystems
                        F:/vcpkg/scripts/buildsystems
                        C:/Users/$ENV{USER}/vcpkg/scripts/buildsystems
                        C:/Users/$ENV{USER}/.vcpkg/scripts/buildsystems
                        /usr/local/vcpkg/scripts/buildsystems
                        /usr/local/share/vcpkg/scripts/buildsystems
                        /src/vcpkg/scripts/buildsystems
                        /home/$ENV{USER}/vcpkg/scripts/buildsystems
                        /home/$ENV{USER}/.vcpkg/scripts/buildsystems
                        /Users/$ENV{USER}/vcpkg/scripts/buildsystems
                        /Users/$ENV{USER}/.vcpkg/scripts/buildsystems
                )

                if (${VCPKG_TOOLCHAIN_DIR} STREQUAL "VCPKG_TOOLCHAIN_DIR-NOTFOUND")
                    message(FATAL_ERROR "Could not found the vcpkg.cmake")
                else ()
                    message(STATUS "VCPKG_TOOLCHAIN_FILE: ${VCPKG_TOOLCHAIN_DIR}/vcpkg.cmake")
                    include("${VCPKG_TOOLCHAIN_DIR}/vcpkg.cmake")
                endif ()
            endif ()
        endif ()
    endif ()
endmacro()

macro(sese_copy_dll_to_binary_path target dest)
    if (${CMAKE_GENERATOR} MATCHES "Ninja")
        # 单配置
        add_custom_command(
                TARGET
                ${target}
                POST_BUILD
                COMMAND
                ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:${target}> ${PROJECT_BINARY_DIR}/${dest}
        )
    else ()
        # 多配置
        add_custom_command(
                TARGET
                ${target}
                POST_BUILD
                COMMAND
                ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:${target}> ${PROJECT_BINARY_DIR}/${dest}/$<CONFIG>
        )
    endif ()
endmacro()