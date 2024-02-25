#include <sese/convert/PercentConverter.h>
#include <sese/net/http/QueryString.h>
#include <sese/text/StringBuilder.h>

using sese::net::http::QueryString;
using sese::text::StringBuilder;

const std::string &QueryString::get(const std::string &key, const std::string &default_value) noexcept {
    auto iterator = keyValueSet.find(key);
    if (iterator == keyValueSet.end()) {
        return default_value;
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
    bool has_key = false;
    for (char ch: query) {
        if (ch == '?') {
            continue;
        } else if (ch == '=') {
            if (has_key) {
                continue;
            } else {
                has_key = true;
                key = builder.toString();
                builder.clear();
            }
        } else if (ch == '&') {
            if (has_key) {
                value = builder.toString();
                builder.clear();
                if (!value.empty()) {
                    auto decode_key = PercentConverter::decode(key.c_str());
                    auto decode_value = PercentConverter::decode(value.c_str());
                    if (!decode_key.empty() && !decode_value.empty()) {
                        keyValueSet[decode_key] = decode_value;
                    }
                }
            }
            has_key = false;
        } else {
            builder.append(ch);
        }
    }
    if (has_key && !builder.empty()) {
        value = builder.toString();
        auto decode_key = PercentConverter::decode(key.c_str());
        auto decode_value = PercentConverter::decode(value.c_str());
        if (decode_key.empty() || decode_value.empty()) {
            return;
        }
        keyValueSet[decode_key] = decode_value;
    }
}

std::string QueryString::toString() noexcept {
    StringBuilder builder;
    if (!keyValueSet.empty()) {
        builder.append('?');

        bool is_first = true;
        for (const auto &item: keyValueSet) {
            if (is_first) {
                is_first = false;
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