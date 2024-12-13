find_package(
    Python3
    COMPONENTS Interpreter
    REQUIRED
)

function(target_manifest target manifest_file)
    set(script manifest.py)
    if(SESE_ROOT_PROJECT)
        set(script ${PROJECT_SOURCE_DIR}/scripts/manifest.py)
    endif()
    if(WIN32)
        message(STATUS "(Manifest) Pre-built for manifest: ${target}(${CMAKE_CURRENT_LIST_DIR}/${manifest_file})")
        get_target_property(filename ${target} OUTPUT_NAME)
        execute_process(
            COMMAND ${Python3_EXECUTABLE} ${script} "--manifest_file_path=${CMAKE_CURRENT_LIST_DIR}/${manifest_file}"
                    "--output_file_path=${CMAKE_BINARY_DIR}/${target}-manifest.rc" "--filename=${filename}"
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
            OUTPUT_QUIET
            RESULT_VARIABLE exit_code
        )
        if(${exit_code} EQUAL 0)
            message(STATUS "(Manifest) Pre-built complete!!!")
            target_sources(${target} PRIVATE "${CMAKE_BINARY_DIR}/${target}-manifest.rc")
        else()
            message(FATAL_ERROR "(Manifest) Failed")
        endif()
    endif()
endfunction()
