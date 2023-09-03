#include <sese/convert/PercentConverter.h>
#include <sese/net/http/QueryString.h>
#include <sese/text/StringBuilder.h>

using sese::net::http::QueryString;
using sese::text::StringBuilder;

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
                    auto decodeKey = PercentConverter::decode(key.c_str());
                    auto decodeValue = PercentConverter::decode(value.c_str());
                    if (!decodeKey.empty() && !decodeValue.empty()) {
                        keyValueSet[decodeKey] = decodeValue;
                    }
                }
            }
            hasKey = false;
        } else {
            builder.append(ch);
        }
    }
    if (hasKey && !builder.empty()) {
        value = builder.toString();
        auto decodeKey = PercentConverter::decode(key.c_str());
        auto decodeValue = PercentConverter::decode(value.c_str());
        if (decodeKey.empty() || decodeValue.empty()) {
            return;
        }
        keyValueSet[decodeKey] = decodeValue;
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
            builder.append(PercentConverter::encode(item.first.c_str()));
            builder.append('=');
            builder.append(PercentConverter::encode(item.second.c_str()));
        }
    }
    return builder.toString();
}