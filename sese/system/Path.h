/// \file Path.h
/// \brief UNIX-LIKE 路径转换类
/// \author kaoru
/// \date 2023年10月26日

#pragma once

#include <sese/Config.h>

#ifdef SESE_PLATFORM_WINDOWS
#pragma warning(disable : 4251)
#endif

namespace sese::system {

/// UNIX-LIKE 路径转换类
class API Path {
public:
    /** \brief 路径构造函数
     *  \verbatim
for Windows:
    /c/Windows -> c:/Windows
    /C/Users   -> C:/Users
    ./Dir      -> ./Dir
    Dir        -> Dir
     *  \endverbatim
     *  \param unixPath UNIX-LIKE 格式路径
     */
    explicit Path(const char *unix_path) noexcept;

    explicit Path(const std::string_view &unix_path) noexcept;

    Path() = default;

    /// 将原生路径转换为 UNIX-LIKE 路径
    /// \warning 不做额外检查
    /// \param nativePath 原生路径
    /// \return UNIX-LIKE 路径
    static Path fromNativePath(const std::string &native_path) noexcept;

#ifdef SESE_PLATFORM_WINDOWS
    static void replaceWindowsPathSplitChar(char *path, size_t len) noexcept;

    [[nodiscard]] const std::string &getNativePath() const { return nativePath; }
#else
    [[nodiscard]] const std::string &getNativePath() const { return unixPath; }
#endif

    [[nodiscard]] const std::string &getUnixPath() const { return unixPath; }

    /// \return 当前路径是否有效
    [[nodiscard]] bool isValid() const { return valid; }

protected:
#ifdef SESE_PLATFORM_WINDOWS
    std::string nativePath{};
#endif
    std::string unixPath{};

    bool valid = false;
};

} // namespace sese::system