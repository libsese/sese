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

/**
 * @brief 请求头类
 * @warning 字段 uri 未进行任何处理（URL + QueryString）
 */
class API RequestHeader : public Header {
public:
    using Ptr = std::unique_ptr<RequestHeader>;

    RequestHeader() = default;

    RequestHeader(const std::initializer_list<KeyValueType> &initializer_list)
        : Header(initializer_list) {}

    [[nodiscard]] RequestType getType() const { return type; }
    void setType(RequestType request_type) { this->type = request_type; }

    [[nodiscard]] const std::string &getUrl() const { return url; }
    void setUrl(const std::string &request_url) { this->url = request_url; }

    [[nodiscard]] HttpVersion getVersion() const { return version; }
    void setVersion(HttpVersion new_version) { this->version = new_version; }

protected:
    RequestType type = RequestType::GET;
    std::string url = "/";
    HttpVersion version = HttpVersion::VERSION_1_1;
};

} // namespace sese::net::http