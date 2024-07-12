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

// GCOVR_EXCL_START

/**
 * @brief 请求头类
 * @warning 字段 uri 未进行任何处理（URL + QueryString）
 */
class  RequestHeader : public Header {
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