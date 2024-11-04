// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <sese/convert/PercentConverter.h>
#include <sese/net/http/RequestHeader.h>
#include <sese/text/StringBuilder.h>

using namespace sese;

const std::string &net::http::RequestHeader::getQueryArg(const std::string &key, const std::string &default_value) const {
    auto iterator = query_args.find(key);
    if (iterator == query_args.end()) {
        return default_value;
    }
    return iterator->second;
}

void net::http::RequestHeader::setQueryArg(const std::string &key, const std::string &value) {
    query_args[key] = value;
}

std::string net::http::RequestHeader::getUrl() const {
    text::StringBuilder builder;
    auto dirs = text::StringBuilder::split(uri, "/");
    if (dirs.size() == 1 && dirs[0].empty()) {
        builder.append('/');
    } else {
        for (auto &&item: dirs) {
            if (!item.empty()) {
                builder.append('/');
                builder.append(PercentConverter::encode(item.c_str()));
            }
        }
    }

    bool first = true;
    for (auto &&item: query_args) {
        if (first) {
            first = false;
            builder.append('?');
        } else {
            builder.append('&');
        }
        builder.append(PercentConverter::encode(item.first.c_str()));
        builder.append('=');
        builder.append(PercentConverter::encode(item.second.c_str()));
    }

    return builder.toString();
}

void net::http::RequestHeader::setUrl(const std::string &request_url) {
    auto pos = request_url.find('?');
    if (pos == std::string::npos) {
        uri = PercentConverter::decode(request_url.c_str());
        return;
    }

    uri = PercentConverter::decode(request_url.substr(0, pos).c_str());
    auto query_string = request_url.substr(pos + 1);
    auto query_items = text::StringBuilder::split(query_string, "&");
    for (auto &&item: query_items) {
        auto pair = text::StringBuilder::split(item, "=");
        if (pair.size() == 1 && !pair.empty()) {
            auto key = PercentConverter::decode(pair[0].c_str());
            query_args[key] = "";
        } else if (pair.size() == 2) {
            auto key = PercentConverter::decode(pair[0].c_str());
            auto value = PercentConverter::decode(pair[1].c_str());
            query_args[key] = value;
        }
        // GCOVR_EXCL_START
        else {
            continue;
        }
        // GCOVR_EXCL_STOP
    }
}
