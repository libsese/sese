include(${PROJECT_SOURCE_DIR}/cmake/FetchContentToolKits.cmake)

requires_from_git(SString https://github.com/libsese/SString.git v0.1.2)

requires_from_git(SimpleUuid https://github.com/libsese/SimpleUuid.git v0.1.2)

option(LIBRESSL_TESTS "" OFF)
option(LIBRESSL_APPS "" OFF)
option(LIBRESSL_SKIP_INSTALL "" ON)
option(BUILD_SHARED_LIBS "" OFF)
requires_from_git(LibreSSL https://github.com/PowerShell/LibreSSL.git V3.6.1.0)
# target_include_directories(ssl-static PUBLIC ${LibreSSL_SOURCE_DIR}/include)
# 此选项会导致在 Darwin 下与同名文件 VERSION 冲突，若有需要请尝试从更高一层的目录进行访问
# target_include_directories(crypto PUBLIC ${LibreSSL_SOURCE_DIR})
if (MSVC)
    # 屏蔽不需要关注的目标的警告
    target_compile_options(crypto_obj PRIVATE /wd4061 /wd4065 /wd4152 /wd4132 /wd4191 /wd4242 /wd4255 /wd4295 /wd4296 /wd4388 /wd4464 /wd4668 /wd4701 /wd4702 /wd4774 /wd4819 /wd4820 /wd5045 /wd5105)
    target_compile_options(ssl_obj PRIVATE /wd4061 /wd4065 /wd4152 /wd4132 /wd4191 /wd4242 /wd4255 /wd4295 /wd4296 /wd4388 /wd4464 /wd4668 /wd4701 /wd4702 /wd4774 /wd4819 /wd4820 /wd5045 /wd5105)
endif ()