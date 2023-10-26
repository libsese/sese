#include <sese/system/Path.h>

using namespace sese::system;

Path::Path(const std::string &unixPath) noexcept {
    Path::unixPath = unixPath;
    Path::valid = true;
}

Path Path::fromNativePath(const std::string &nativePath) noexcept {
    Path result;
    result.unixPath = nativePath;
    result.valid = true;
    return result;
}