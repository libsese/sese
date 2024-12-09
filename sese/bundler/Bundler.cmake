macro(target_include_resource target dest resource_file)
    add_custom_target(
            ${target}Resources ALL
            COMMAND $<TARGET_FILE:Bundler>
            "--base_path=${CMAKE_CURRENT_LIST_DIR}"
            "--resource_file_path=${resource_file}"
            "--generate_code_path=${dest}"
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    )
    file(GLOB resource_files "${dest}/*.rc" "${dest}/*.h")
    target_sources(${target} PRIVATE ${resource_files})
    target_include_directories(${target} PRIVATE ${dest})

    add_dependencies(${target}Resources Bundler)
    add_dependencies(${target} ${target}Resources)
endmacro()