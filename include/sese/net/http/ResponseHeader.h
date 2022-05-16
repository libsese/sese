#pragma once
#include <sese/net/http/Header.h>

namespace sese::http {

    /**
     * @brief 响应头类
     */
    class API ResponseHeader : public Header {
    public:
        using Ptr = std::shared_ptr<ResponseHeader>;

        ResponseHeader() = default;

        ResponseHeader(const std::initializer_list<std::pair<const std::string &, const std::string &>> &initializerList)
            : Header(initializerList) {}

        void setCode(uint16_t responseCode) noexcept { statusCode = responseCode; }
        [[nodiscard]] uint16_t getCode() const noexcept { return statusCode; }

        [[nodiscard]] HttpVersion getVersion() const { return version; }
        void setVersion(HttpVersion newVersion) { this->version = newVersion; }

    protected:
        uint16_t statusCode = 200;
        HttpVersion version = HttpVersion::VERSION_1_1;
    };

}// namespace sese::http