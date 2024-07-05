function(remove_empty_directories_recursive dir)
    file(GLOB before_subdirs "${dir}/*")
    foreach (subdir ${before_subdirs})
        if (IS_DIRECTORY ${subdir})
            remove_empty_directories_recursive(${subdir})
        endif ()
    endforeach ()
    file(GLOB after_subdirs "${dir}/*")
    if ("${after_subdirs}" STREQUAL "")
        file(REMOVE_RECURSE "${dir}")
    endif ()
endfunction()

set(SOURCE_PATH ${CURRENT_BUILDTRESS_DIR}/sese)

vcpkg_from_github(
        OUT_SOURCE_PATH SOURCE_PATH
        REPO libsese/sese
        REF 29cc110ad612bcf0b4ac1b686505c43e22ce1877
        SHA512 ab5dd2b1ea48a1cf7ca77f8ce11b09f942f0c098d3d732faa346418b8f56395113659603c6a9ef6a4122959f264ed16debd3e1da0bf034466797e33213295785
)

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS
        FEATURES
        tests        SESE_BUILD_TEST
        mysql        SESE_DB_USE_MARIADB
        sqlite3      SESE_DB_USE_SQLITE
        psql         SESE_DB_USE_POSTGRES
        async-logger SESE_USE_ASYNC_LOGGER
        archive      SESE_USE_ARCHIVE
)

vcpkg_cmake_configure(
        SOURCE_PATH "${SOURCE_PATH}"
        OPTIONS ${FEATURE_OPTIONS}
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(CONFIG_PATH "lib/cmake/sese")

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

remove_empty_directories_recursive("${CURRENT_PACKAGES_DIR}/include/sese")

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE" "${SOURCE_PATH}/NOTICE")

file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")