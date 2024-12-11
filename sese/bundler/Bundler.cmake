find_package(Python3 COMPONENTS Interpreter REQUIRED)

macro(target_include_resource target dest resource_file)
    if (WIN32)
        set(method "rc")
    elseif (LINUX)
        set(method "elf")
    elseif (APPLE)
        set(method "mach-o")
    else ()
        message(FATAL_ERROR "Unsupported file formats")
    endif ()
    get_filename_component(name ${resource_file} NAME_WE)
    execute_process(
            COMMAND ${Python3_EXECUTABLE} ${PROJECT_SOURCE_DIR}/scripts/bundler.py
            "--base_path=${CMAKE_CURRENT_LIST_DIR}"
            "--resource_file_path=${resource_file}"
            "--generate_code_path=${dest}"
            "--method_name=${method}"
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    )
    if (WIN32)
        target_sources(${target} PRIVATE "${dest}/${name}.rc" "${dest}/${name}.h")
    elseif (LINUX)
        target_sources(${target} PRIVATE "${dest}/${name}.o" "${dest}/${name}.cpp" "${dest}/${name}.h")
    elseif (APPLE)
    endif ()
    target_include_directories(${target} PRIVATE ${dest})
endmacro()