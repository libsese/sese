#include <sese/system/Path.h>
#include <sese/text/StringBuilder.h>

#include <cctype>

using namespace sese::system;

Path::Path(const std::string_view &unixPath) noexcept {
    Path::unixPath = unixPath;

    // 带盘符的绝对路径
    if (!Path::unixPath.empty() && Path::unixPath.at(0) == '/') {
        auto pos = Path::unixPath.find('/', 1);
        if (pos == std::string::npos) {
            valid = false;
            return;
        }
        if (pos - 1 != 1) {
            valid = false;
            return;
        }
        auto driver = Path::unixPath.substr(1, pos - 1);
        nativePath = driver + ':' + (Path::unixPath.c_str() + 2);
        valid = true;
    }
    // 相对路径
    else {
        nativePath = unixPath;
        valid = true;
    }
}

Path::Path(const char *unixPath) noexcept : Path(std::string_view(unixPath)) {
}

Path Path::fromNativePath(const std::string &nativePath) noexcept {
    // 带盘符的绝对路径
    if (nativePath.size() >= 3 && nativePath.compare(1, 2, ":/") == 0 && isalpha(nativePath.at(0))) {
        text::StringBuilder stringBuilder;
        stringBuilder.append('/');
        stringBuilder.append(nativePath.at(0));
        stringBuilder.append(nativePath.c_str() + 2, nativePath.size() - 2);

        Path result;
        result.unixPath = stringBuilder.toString();
        result.nativePath = nativePath;
        result.valid = true;
        return result;
    }
    // 相对路径
    else if (!nativePath.empty() && nativePath.at(0) != '/') {
        Path result;
        result.unixPath = nativePath;
        result.nativePath = nativePath;
        result.valid = true;
        return result;
    }
    // 非法路径
    else {
        return {};
    }
}