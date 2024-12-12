# ######################################################################################################################
# CMake 安装包配置
# ######################################################################################################################
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/sese-config.cmake.in ${PROJECT_BINARY_DIR}/sese-config.cmake
    INSTALL_DESTINATION lib/cmake/sese
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
    DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.hpp"
    PATTERN "${CMAKE_CURRENT_LIST_DIR}/internal" EXCLUDE
    PATTERN "${CMAKE_CURRENT_LIST_DIR}/test" EXCLUDE
    PATTERN "${CMAKE_CURRENT_LIST_DIR}/example" EXCLUDE
)

install(FILES "${PROJECT_BINARY_DIR}/sese-config.cmake" DESTINATION lib/cmake/sese)

install(FILES "${PROJECT_BINARY_DIR}/sese-config.cmake" DESTINATION debug/lib/cmake/sese)

install(
    EXPORT SeseTargets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sese
    NAMESPACE Sese::
)
