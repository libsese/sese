# ######################################################################################################################
# CMake Package Configurations
# ######################################################################################################################
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/sese-config.cmake.in ${PROJECT_BINARY_DIR}/sese-config.cmake
    INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
)

install(
    TARGETS ${ARCHIVE_TARGET} DB Core
    EXPORT SeseTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)

install(
    DIRECTORY "${PROJECT_SOURCE_DIR}/sese"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.hpp"
    PATTERN "${PROJECT_SOURCE_DIR}/sese/test" EXCLUDE
    PATTERN "${PROJECT_SOURCE_DIR}/sese/example" EXCLUDE
)

install(FILES "${PROJECT_BINARY_DIR}/sese-config.cmake" DESTINATION lib/cmake/${PROJECT_NAME})

install(FILES "${PROJECT_BINARY_DIR}/sese-config.cmake" DESTINATION debug/lib/cmake/${PROJECT_NAME})

install(FILES "${PROJECT_SOURCE_DIR}/cmake/Manifest.cmake" "${PROJECT_SOURCE_DIR}/scripts/manifest.py"
              "${PROJECT_SOURCE_DIR}/cmake/Bundler.cmake" "${PROJECT_SOURCE_DIR}/scripts/bundler.py"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)

install(
    EXPORT SeseTargets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    NAMESPACE Sese::
)
