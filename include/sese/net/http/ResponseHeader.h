#pragma once
#include <sese/net/http/Header.h>

namespace sese::http {

    /**
     * @brief 响应头类
     */
    class API ResponseHeader : public Header {
    public:
        ResponseHeader(std::initializer_list<std::pair<const std::string &, const std::string &>> initializerList)
            : Header(initializerList) {}

        void setCode(uint16_t responseCode) noexcept { statusCode = responseCode; }
        [[nodiscard]] uint16_t getCode() const noexcept { return statusCode; }

    protected:
        uint16_t statusCode = 200;
    };

}// namespace sese::http