cmake_minimum_required(VERSION 3.14)

include(FetchContent)

set(FETCHCONTENT_QUIET off)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif ()

macro(requires_from_git name url hash_or_tag)
    FetchContent_Declare(
            ${name}
            GIT_REPOSITORY
            ${url}
            GIT_TAG
            ${hash_or_tag}
    )

    FetchContent_MakeAvailable(${name})
endmacro()

macro(requires_from_url name url sha256)
    FetchContent_Declare(
            ${name}
            URL
            ${url}
            URL_HASH SHA3_256=${sha256}
    )

    FetchContent_MakeAvailable(${name})
endmacro()