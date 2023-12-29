#include <sese/db/Util.h>
#include <sese/text/StringBuilder.h>

using sese::text::StringBuilder;

std::map<std::string, std::string> sese::db::tokenize(const char *p) noexcept {
    std::map<std::string, std::string> result;

    auto str = StringBuilder::split(p, ";");
    for (auto &&item: str) {
        auto pair = StringBuilder::split(item, "=");
        auto &&key = pair[0];
        auto &&value = pair[1];
        result[key] = value;
    }

    return result;
}