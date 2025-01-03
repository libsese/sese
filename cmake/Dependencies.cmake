list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(Git)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(asio CONFIG REQUIRED)
find_package(simdjson CONFIG REQUIRED)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_package(Threads REQUIRED)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    find_library(IOKIT_FRAMEWORK IOKit REQUIRED)
    find_library(CORESERVICES_FRAMEWORK CoreServices REQUIRED)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation REQUIRED)
endif()

if(SESE_REPLACE_EXECINFO)
    find_package(PkgConfig)
    pkg_check_modules(libunwind REQUIRED IMPORTED_TARGET GLOBAL libunwind)
endif()

if(SESE_USE_ARCHIVE)
    find_package(LibArchive REQUIRED)
endif()

if(SESE_DB_USE_SQLITE)
    find_package(unofficial-sqlite3 CONFIG REQUIRED)
endif()

if(SESE_DB_USE_MARIADB)
    find_package(unofficial-libmariadb CONFIG REQUIRED)
endif()

if(SESE_DB_USE_POSTGRES)
    find_package(PostgreSQL REQUIRED)
endif()

if(SESE_BUILD_TEST)
    # https://discourse.cmake.org/t/possible-to-create-a-python-virtual-env-from-cmake-and-then-find-it-with-findpython3/1132/2
    set(ENV{VIRTUAL_ENV} "${PROJECT_SOURCE_DIR}/venv")
    set(Python3_FIND_VIRTUALENV FIRST)
    # 已知此功能在 GitHub Action 中的 Windows 平台下不符合预期，无法选中对应的虚拟环境 如有需要请指定环境变量 Python3_ROOT_DIR
    find_package(
        Python3
        COMPONENTS Interpreter
        REQUIRED
    )
    find_package(GTest REQUIRED)
    include_directories(GoogleTest)
endif()
