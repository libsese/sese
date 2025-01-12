find_package(
    Python3
    COMPONENTS Interpreter
    REQUIRED
)

function(target_include_resource target dest resource_file)
    set(script ${SESE_PACKAGE_DIR}/bundler.py)
    if(SESE_ROOT_PROJECT)
        set(script ${PROJECT_SOURCE_DIR}/scripts/bundler.py)
    endif()
    get_target_property(type ${target} TYPE)
    if(NOT type STREQUAL "EXECUTABLE")
        message(FATAL_ERROR "(Bundler) The target must be an executable!!!")
    endif()
    if(WIN32)
        set(method "rc")
    elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
        set(method "elf")
    elseif(APPLE)
        set(method "mach-o")
    else()
        message(FATAL_ERROR "Unsupported file formats!!!")
    endif()
    get_filename_component(name ${resource_file} NAME_WE)
    message(STATUS "(Bundler) Pre-built for resources: ${target}(${CMAKE_CURRENT_LIST_DIR}/${resource_file})")
    execute_process(
        COMMAND ${Python3_EXECUTABLE} ${script} "--base_path=${CMAKE_CURRENT_LIST_DIR}"
                "--resource_file_path=${resource_file}" "--generate_code_path=${dest}" "--method_name=${method}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        OUTPUT_QUIET
        RESULT_VARIABLE exit_code
    )
    if(${exit_code} EQUAL 0)
        message(STATUS "(Bundler) Pre-built complete!!!")
    else()
        message(FATAL_ERROR "(Bundler) Pre-built failed")
    endif()
    if(WIN32)
        target_sources(${target} PRIVATE "${dest}/${name}.rc" "${dest}/${name}.h")
    elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
        target_sources(${target} PRIVATE "${dest}/${name}.o" "${dest}/${name}.cpp" "${dest}/${name}.h")
        target_link_options(${target} PRIVATE -z noexecstack)
    elseif(APPLE)
        target_sources(${target} PRIVATE "${dest}/${name}.h")
        file(READ "${dest}/${name}.link_options" file_contents)
        foreach(line IN LISTS file_contents)
            target_link_options(${target} PRIVATE LINKER:-sectcreate,__DATA,${line})
        endforeach()
    endif()
    target_include_directories(${target} PRIVATE ${dest})
    unset(script)
endfunction()
