include(${PROJECT_SOURCE_DIR}/cmake/FetchContentToolKits.cmake)

requires_from_git(SString https://github.com/libsese/SString.git v0.1.2)

requires_from_git(SimpleUuid https://github.com/libsese/SimpleUuid.git v0.1.1)

option(LIBRESSL_TESTS "" OFF)
option(LIBRESSL_APPS "" OFF)
option(LIBRESSL_SKIP_INSTALL "" ON)
option(BUILD_SHARED_LIBS "" ON)
requires_from_git(LibreSSL https://github.com/PowerShell/LibreSSL.git V3.6.1.0)
target_include_directories(ssl PUBLIC ${LibreSSL_SOURCE_DIR}/include)
target_include_directories(crypto PUBLIC ${LibreSSL_SOURCE_DIR})