/**
 * @file ResponseHeader.h
 * @author your name (you@domain.com)
 * @brief 响应头类
 * @date 2022年05月17日
 */
#pragma once
#include <sese/net/http/Header.h>

namespace sese::net::http {

/**
 * @brief 响应头类
 */
class API ResponseHeader : public Header {
public:
    using Ptr = std::unique_ptr<ResponseHeader>;

    ResponseHeader() = default;

    ResponseHeader(const std::initializer_list<KeyValueType> &initializer_list)
        : Header(initializer_list) {}

    void setCode(uint16_t response_code) noexcept { statusCode = response_code; }
    [[nodiscard]] uint16_t getCode() const noexcept { return statusCode; }

    [[nodiscard]] HttpVersion getVersion() const { return version; }
    void setVersion(HttpVersion new_version) { this->version = new_version; }

protected:
    uint16_t statusCode = 200;
    HttpVersion version = HttpVersion::VERSION_1_1;
};

} // namespace sese::net::http