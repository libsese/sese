#include <sese/system/Path.h>

using namespace sese::system;

Path::Path(const std::string_view &unix_path) noexcept {
    Path::unix_path = unix_path;
    Path::valid = true;
}

Path::Path(const char *unix_path) noexcept : Path(std::string_view(unix_path)) {
}

Path Path::fromNativePath(const std::string &native_path) noexcept {
    Path result;
    result.unix_path = native_path;
    result.valid = true;
    return result;
}