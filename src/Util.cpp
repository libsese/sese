#include <sese/db/Util.h>
#include <sstream>

std::map<std::string, std::string> sese::db::tokenize(const char *p) noexcept {
    std::map<std::string, std::string> result;
    std::stringstream stream;
    std::string key, value;

    /// 用于指示当前查找的是否是 key
    bool flag = true;
    while(*p != 0) {
        if (flag) {
            if (*p != '=') {
                stream.write(p, 1);
                p++;
            } else {
                p++;
                key = stream.str();
                stream.clear();
                stream.str("");
                flag = false;
            }
        } else {
            if (*p != ';') {
                stream.write(p, 1);
                p++;
            } else {
                p++;
                value = stream.str();
                stream.clear();
                stream.str("");
                flag = true;
                result[key] = value;
            }
        }
    }

    return result;
}