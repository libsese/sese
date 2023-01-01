#include <sese/net/http/QueryString.h>
#include <sese/text/StringBuilder.h>

using sese::text::StringBuilder;
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

QueryString::QueryString(const std::string &query) noexcept {
    StringBuilder builder;

    // 仅有 '?'
    if (query.size() == 1) {
        return;
    }

    std::string key;
    std::string value;
    bool hasKey = false;
    for (char ch: query) {
        if (ch == '?') {
            continue;
        } else if (ch == '=') {
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

std::string QueryString::toString() noexcept {
    StringBuilder builder;
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