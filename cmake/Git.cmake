macro(get_git_hash _git_hash)
    find_package(Git)
    if (GIT_FOUND)
        execute_process(
                COMMAND ${GIT_EXECUTABLE} --no-pager log -1 --pretty=format:%h
                OUTPUT_VARIABLE ${_git_hash}
                OUTPUT_STRIP_TRAILING_WHITESPACE
                WORKING_DIRECTORY ..
                RESULT_VARIABLE GIT_RESULT
        )
        if (NOT GIT_RESULT EQUAL "0")
            # message(FATAL_ERROR "failed to get the git repository hash value")
        endif ()
    endif ()
endmacro()

macro(get_git_branch _git_branch)
    find_package(Git)
    if (GIT_FOUND)
        execute_process(
                COMMAND ${GIT_EXECUTABLE} --no-pager symbolic-ref --short -q HEAD
                OUTPUT_VARIABLE ${_git_branch}
                OUTPUT_STRIP_TRAILING_WHITESPACE
                WORKING_DIRECTORY ..
                RESULT_VARIABLE GIT_RESULT
        )
        if (NOT GIT_RESULT EQUAL "0")
            message(FATAL_ERROR "failed to get the git repository branch name")
        endif ()
    endif ()
endmacro()

macro(git_update_submodule)
    find_package(Git)
    if (GIT_FOUND)
        execute_process(
                COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                WORKING_DIRECTORY ..
                RESULT_VARIABLE GIT_RESULT
        )
        if (NOT GIT_RESULT EQUAL "0")
            message(FATAL_ERROR "failed to update the git submodule")
        endif ()
    endif ()
endmacro()