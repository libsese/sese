macro(get_git_hash _git_hash work_dir)
    if (GIT_FOUND)
        execute_process(
                COMMAND ${GIT_EXECUTABLE} -C ${work_dir} --no-pager log -1 --pretty=format:%h
                OUTPUT_VARIABLE ${_git_hash}
                OUTPUT_STRIP_TRAILING_WHITESPACE
                WORKING_DIRECTORY .
                RESULT_VARIABLE GIT_RESULT
        )
        if (NOT GIT_RESULT EQUAL "0")
            message(FATAL_ERROR "failed to get the git repository hash value")
        endif ()
    endif ()
endmacro()

macro(get_git_branch _git_branch work_dir)
    if (GIT_FOUND)
        execute_process(
                COMMAND ${GIT_EXECUTABLE} -C ${work_dir} --no-pager symbolic-ref --short -q HEAD
                OUTPUT_VARIABLE ${_git_branch}
                OUTPUT_STRIP_TRAILING_WHITESPACE
                WORKING_DIRECTORY .
                RESULT_VARIABLE GIT_RESULT
        )
        if (NOT GIT_RESULT EQUAL "0")
            # message(FATAL_ERROR "failed to get the git repository branch name")
            set(${_git_branch} "Unknown")
        endif ()
    endif ()
endmacro()

macro(git_update_submodule work_dir)
    if (GIT_FOUND)
        execute_process(
                COMMAND ${GIT_EXECUTABLE} -C ${work_dir} submodule update --init --recursive
                WORKING_DIRECTORY .
                RESULT_VARIABLE GIT_RESULT
        )
        if (NOT GIT_RESULT EQUAL "0")
            message(FATAL_ERROR "failed to update the git submodule")
        endif ()
    endif ()
endmacro()

set(SESE_REPO_HASH "UNKNOWN")
set(SESE_REPO_BRANCH "UNKNOWN")
if (EXISTS ${PROJECT_SOURCE_DIR}/.git)
    get_git_hash(SESE_REPO_HASH ${CMAKE_CURRENT_LIST_DIR})
    get_git_branch(SESE_REPO_BRANCH ${CMAKE_CURRENT_LIST_DIR})
endif ()