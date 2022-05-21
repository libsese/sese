/**
 * @file RequestHeader.h
 * @author kaoru
 * @brief 请求头类
 * @date 2022年05月17日
 */
#pragma once
#include <sese/net/http/Header.h>

namespace sese::http {

    /// @brief 请求类型
    enum class RequestType {
        Get,
        Post,
        Another
    };

    /**
     * @brief 请求头类
     * @warning 字段 uri 未进行任何处理（URL + QueryString）
     */
    class API RequestHeader : public Header {
    public:
        using Ptr = std::shared_ptr<RequestHeader>;

        RequestHeader() = default;

        RequestHeader(const std::initializer_list<KeyValueType> &initializerList)
            : Header(initializerList) {}

        [[nodiscard]] RequestType getType() const { return type; }
        void setType(RequestType requestType) { this->type = requestType; }

        [[nodiscard]] const std::string &getUrl() const { return url; }
        void setUrl(const std::string &requestUrl) { this->url = requestUrl; }

        [[nodiscard]] HttpVersion getVersion() const { return version; }
        void setVersion(HttpVersion newVersion) { this->version = newVersion; }

    protected:
        RequestType type = RequestType::Get;
        std::string url = "/";
        HttpVersion version = HttpVersion::VERSION_1_1;
    };

}// namespace sese::http