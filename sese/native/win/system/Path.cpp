#include <sese/system/Path.h>
#include <sese/text/StringBuilder.h>

#include <cctype>

using namespace sese::system;

Path::Path(const std::string_view &unix_path) noexcept {
    Path::unixPath = unix_path;

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
        nativePath = unix_path;
        valid = true;
    }
}

Path::Path(const char *unix_path) noexcept : Path(std::string_view(unix_path)) {
}

Path Path::fromNativePath(const std::string &native_path) noexcept {
    // 带盘符的绝对路径
    if (native_path.size() >= 3 && native_path.compare(1, 2, ":/") == 0 && isalpha(native_path.at(0))) {
        text::StringBuilder string_builder;
        string_builder.append('/');
        string_builder.append(native_path.at(0));
        string_builder.append(native_path.c_str() + 2, native_path.size() - 2);

        Path result;
        result.unixPath = string_builder.toString();
        result.nativePath = native_path;
        result.valid = true;
        return result;
    }
    // 相对路径
    else if (!native_path.empty() && native_path.at(0) != '/') {
        Path result;
        result.unixPath = native_path;
        result.nativePath = native_path;
        result.valid = true;
        return result;
    }
    // 非法路径
    else {
        return {};
    }
}

void Path::replaceWindowsPathSplitChar(char *path, size_t len) noexcept {
    for (size_t i = 0; i < len; i++) {
        if (path[i] == '\\') {
            path[i] = '/';
        }
    }
}