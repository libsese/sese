#include <sese/net/http/QueryString.h>
#include <sese/StringBuilder.h>

using sese::StringBuilder;
using sese::http::QueryString;

const std::string &QueryString::get(const std::string &key, const std::string &defaultValue) noexcept {
    auto iterator = keyValueSet.find(key);
    if (iterator == keyValueSet.end()) {
        return defaultValue;
    } else {
        return iterator->second;
    }
}

void QueryString::set(const std::string &key, const std::string &value) noexcept {
    keyValueSet[key] = value;
}

void QueryString::parse(const std::string &rawUrl) noexcept {
    StringBuilder builder;
    bool hasQueryString = false;
    auto start = 0;
    for (char ch: rawUrl) {
        start++;
        if (ch == '?') {
            hasQueryString = true;
            break;
        } else {
            builder.append(ch);
        }
    }
    url = builder.toString();

    if (hasQueryString) {
        builder.clear();

        std::string_view view{rawUrl.c_str() + start, rawUrl.size() - start};
        // 仅有 '?'
        if (view.size() == 1) {
            return;
        }

        std::string key;
        std::string value;
        bool hasKey = false;
        for (char ch: view) {
            if (ch == '=') {
                if (hasKey) {
                    continue;
                } else {
                    hasKey = true;
                    key = builder.toString();
                    builder.clear();
                }
            } else if (ch == '&') {
                if (hasKey) {
                    value = builder.toString();
                    builder.clear();
                    if (!value.empty()) {
                        keyValueSet[key] = value;
                    }
                }
                hasKey = false;
            } else {
                builder.append(ch);
            }
        }
        if (hasKey && !builder.empty()) {
            keyValueSet[key] = builder.toString();
        }
    }
}

std::string QueryString::toString() noexcept {
    StringBuilder builder;
    builder.append(url);
    if (!keyValueSet.empty()) {
        builder.append('?');

        bool isFirst = true;
        for (const auto &item: keyValueSet) {
            if (isFirst) {
                isFirst = false;
            } else {
                builder.append('&');
            }
            builder.append(item.first);
            builder.append('=');
            builder.append(item.second);
        }
    }
    return builder.toString();
}