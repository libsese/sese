#pragma once
#include <sese/net/http/Header.h>

namespace sese::http {

    enum class RequestType {
        Get,
        Post,
        Another
    };

    /**
     * @brief 请求头类
     * @warning 字段 url 未进行任何处理（URL + QueryString）
     */
    class API RequestHeader : public Header {
    public:
        RequestHeader(std::initializer_list<std::pair<const std::string &, const std::string &>> initializerList)
            : Header(initializerList) {}

        [[nodiscard]] RequestType getType() const { return type; }
        void setType(RequestType requestType) { this->type = requestType; }

        [[nodiscard]] const std::string &getUrl() const { return url; }
        void setUrl(const std::string &requestUrl) { this->url = requestUrl; }

    protected:
        RequestType type = RequestType::Get;
        std::string url;
    };

}// namespace sese::http