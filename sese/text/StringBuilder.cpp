#include <sese/text/StringBuilder.h>

using sese::text::StringBuilder;

StringBuilder &operator<<(StringBuilder &stream, char ch) noexcept {
    stream.append(ch);
    return stream;
}

StringBuilder &operator<<(StringBuilder &stream, const char *str) noexcept {
    stream.append(str);
    return stream;
}

StringBuilder &operator<<(StringBuilder &stream, const std::string &str) noexcept {
    stream.append(str);
    return stream;
}

StringBuilder &operator<<(StringBuilder &stream, const std::string_view &str) noexcept {
    stream.append(str);
    return stream;
}

StringBuilder &operator<<(StringBuilder &stream, const sese::text::String &str) noexcept {
    stream.append(str);
    return stream;
}

StringBuilder &operator<<(StringBuilder &stream, const sese::text::StringView &str) noexcept {
    stream.append(str);
    return stream;
}