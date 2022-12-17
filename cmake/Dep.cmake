include(${PROJECT_SOURCE_DIR}/cmake/FetchContentToolKits.cmake)

requires_from_git(SString https://github.com/SHIINASAMA/SString.git v0.1.2)

if (SESE_USE_SSL)
    option(LIBRESSL_TESTS "" off)
    option(LIBRESSL_APPS "" off)
    option(BUILD_SHARED_LIBS "" on)
    requires_from_git(LibreSSL https://github.com/PowerShell/LibreSSL.git V3.6.1.0)
    target_include_directories(ssl PUBLIC ${LibreSSL_SOURCE_DIR}/include)
    target_include_directories(crypto PUBLIC ${LibreSSL_SOURCE_DIR})
endif ()

if (SESE_USE_DB)
    requires_from_git(sese-db https://github.com/shiinasama/sese-db.git v0.1.1)
endif ()