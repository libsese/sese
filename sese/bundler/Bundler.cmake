macro(target_include_resource target dest resource_file)
    set(method "ld")
    if (WIN32)
        set(method "rc")
    endif ()
    get_filename_component(name ${resource_file} NAME_WE)
    add_custom_target(
            ${target}Resources ALL
            COMMAND $<TARGET_FILE:Bundler>
            "--base_path=${CMAKE_CURRENT_LIST_DIR}"
            "--resource_file_path=${resource_file}"
            "--generate_code_path=${dest}"
            "--method_name=${method}"
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    )
    if (WIN32)
        file(TOUCH "${dest}/${name}.rc" "${dest}/${name}.h")
        target_sources(${target} PRIVATE "${dest}/${name}.rc" "${dest}/${name}.h")
    else ()
        file(GLOB resource_files "${dest}/${name}.o" "${dest}/${name}.cpp" "${dest}/${name}.h")
        target_sources(${target} PRIVATE ${resource_files})
    endif ()
    target_include_directories(${target} PRIVATE ${dest})
    add_dependencies(${target}Resources Bundler)
    add_dependencies(${target} ${target}Resources)
endmacro()