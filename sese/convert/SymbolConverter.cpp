#include <sese/convert/SymbolConverter.h>
#include <sese/text/StringBuilder.h>

inline bool isNumber(char ch) {
    return ch >= 48 && ch <= 57;
}

std::string sese::SymbolConverter::decodeUnixClassName(const std::type_info *type) noexcept {
    std::string name = type->name();
    text::StringBuilder builder;
    if (name[0] == 'N') {
        // namespace class name
        name = name.substr(1, name.length() - 2);
    }
    if (isNumber(name[0])) {
        // class name
        bool first = true;
        char *p_str = const_cast<char *>(name.data());
        char *p_next = nullptr;
        while (true) {
            auto len = std::strtoll(p_str, &p_next, 10);
            if (len == 0) {
                break;
            }
            if (first) {
                first = false;
            } else {
                builder.append("::");
            }
            builder.append(p_next, len);
            p_str = p_next + len;
        }
    }
    return builder.toString();
}

std::string sese::SymbolConverter::decodeMSVCClassName(const std::type_info *type) noexcept {
    return type->name() + 6;
}