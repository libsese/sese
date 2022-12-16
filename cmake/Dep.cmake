include(FetchContent)

FetchContent_Declare(SString
        GIT_REPOSITORY https://github.com/SHIINASAMA/SString.git
        GIT_TAG v0.1.2
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/lib/SString
        )
FetchContent_GetProperties(SString)
if(NOT SString_POPULATED)
    FetchContent_MakeAvailable(SString)
endif()

if (SESE_USE_SSL)
    option(LIBRESSL_TESTS "" off)
    option(LIBRESSL_APPS "" off)
    option(BUILD_SHARED_LIBS "" on)
    FetchContent_Declare(LibreSSL
            GIT_REPOSITORY https://github.com/PowerShell/LibreSSL.git
            GIT_TAG V3.6.1.0
            SOURCE_DIR ${CMAKE_SOURCE_DIR}/lib/LibreSSL
            )
    FetchContent_GetProperties(LibreSSL)
    if(NOT LibreSSL_POPULATED)
        FetchContent_MakeAvailable(LibreSSL)
    endif()
    target_include_directories(ssl PUBLIC ${LibreSSL_SOURCE_DIR}/include)
    target_include_directories(crypto PUBLIC ${LibreSSL_SOURCE_DIR})
endif ()

if (SESE_USE_DB)
    FetchContent_Declare(sese-db
            GIT_REPOSITORY https://github.com/shiinasama/sese-db.git
            GIT_TAG v0.1.1
            SOURCE_DIR ${CMAKE_SOURCE_DIR}/lib/sese-db
            )
    FetchContent_GetProperties(sese-db)
    if(NOT sese-db_POPULATED)
        FetchContent_MakeAvailable(sese-db)
    endif()
endif ()