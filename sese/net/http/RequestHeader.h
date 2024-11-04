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

/**
 * @file RequestHeader.h
 * @author kaoru
 * @brief 请求头类
 * @date 2022年05月17日
 */
#pragma once

#include <sese/net/http/Header.h>

#ifdef DELETE
#undef DELETE
#endif

namespace sese::net::http {

/// @brief 请求类型
enum class RequestType {
    OPTIONS,
    GET,
    POST,
    HEAD,
    PUT,
    DELETE,
    TRACE,
    CONNECT,
    ANOTHER
};

inline std::string requestTypeToString(RequestType request_type) {
    switch (request_type) {
        case RequestType::OPTIONS:
            return "OPTIONS";
        case RequestType::GET:
            return "GET";
        case RequestType::POST:
            return "POST";
        case RequestType::HEAD:
            return "HEAD";
        case RequestType::PUT:
            return "PUT";
        case RequestType::DELETE:
            return "DELETE";
        case RequestType::TRACE:
            return "TRACE";
        case RequestType::CONNECT:
            return "CONNECT";
        case RequestType::ANOTHER:
        default:
            return "ANOTHER";
    }
}

inline RequestType stringToRequestType(const std::string &request_type_str) {
    if (request_type_str == "OPTIONS") {
        return RequestType::OPTIONS;
    }
    if (request_type_str == "GET") {
        return RequestType::GET;
    }
    if (request_type_str == "POST") {
        return RequestType::POST;
    }
    if (request_type_str == "HEAD") {
        return RequestType::HEAD;
    }
    if (request_type_str == "PUT") {
        return RequestType::PUT;
    }
    if (request_type_str == "DELETE") {
        return RequestType::DELETE;
    }
    if (request_type_str == "TRACE") {
        return RequestType::TRACE;
    }
    if (request_type_str == "CONNECT") {
        return RequestType::CONNECT;
    }
    return RequestType::ANOTHER;
}
// GCOVR_EXCL_START

/**
 * @brief 请求头类
 * @warning 字段 uri 未进行任何处理（URL + QueryString）
 */
class RequestHeader : public Header {
public:
    using Ptr = std::unique_ptr<RequestHeader>;

    RequestHeader() = default;

    RequestHeader(const std::initializer_list<KeyValueType> &initializer_list)
        : Header(initializer_list) {}

    [[nodiscard]] RequestType getType() const { return type; }
    void setType(RequestType request_type) { this->type = request_type; }

    [[nodiscard]] const std::string &getUri() const { return uri; }
    void setUri(const std::string &uri) { this->uri = uri; }

    [[nodiscard]] const std::string &getQueryArg(const std::string &key, const std::string &default_value) const;
    void setQueryArg(const std::string &key, const std::string &value);
    [[nodiscard]] size_t queryArgsSize() const { return query_args.size(); }
    [[nodiscard]] bool queryArgsEmpty() const { return query_args.empty(); }
    void queryArgsClear() { return query_args.clear(); }
    bool queryArgsExist(const std::string &key) { return query_args.find(key) != query_args.end(); }
    /// 当确定一定存在此字段时可以调用
    /// @see sese::net::http::RequestHeader::queryArgsExist
    /// @param key 查询字符串字段名称
    /// @return 值
    const std::string &getQueryArg(const std::string &key) { return query_args.at(key); }

    [[nodiscard]] HttpVersion getVersion() const { return version; }
    void setVersion(HttpVersion new_version) { this->version = new_version; }

    [[nodiscard]] std::string getUrl() const;
    void setUrl(const std::string &request_url);

protected:
    RequestType type = RequestType::GET;
    std::string uri = "/";
    HttpVersion version = HttpVersion::VERSION_1_1;

    std::map<std::string, std::string> query_args;
};

// GCOVR_EXCL_STOP

} // namespace sese::net::http