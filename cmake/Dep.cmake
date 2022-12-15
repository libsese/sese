include(FetchContent)

FetchContent_Declare(SString
        GIT_REPOSITORY https://github.com/SHIINASAMA/SString.git
        GIT_TAG v0.1.2
        )
FetchContent_MakeAvailable(SString)

option(LIBRESSL_TESTS "" off)
option(LIBRESSL_APPS "" off)
option(BUILD_SHARED_LIBS "" on)
FetchContent_Declare(LibreSSL
        GIT_REPOSITORY https://github.com/PowerShell/LibreSSL.git
        GIT_TAG V3.6.1.0
        )
FetchContent_MakeAvailable(LibreSSL)
target_include_directories(ssl PUBLIC ${LibreSSL_SOURCE_DIR}/include)
target_include_directories(crypto PUBLIC ${LibreSSL_SOURCE_DIR})