/**
 * @file HttpUtil.h
 * @brief HTTP 工具类
 * @author kaoru
 * @date 2022年5月17日
 */

#pragma once

#include <sese/net/http/RequestHeader.h>
#include <sese/net/http/ResponseHeader.h>
#include <sese/text/StringBuilder.h>
#include <sese/io/Stream.h>

namespace sese::net::http {

/// @brief HTTP 工具类
class  HttpUtil {
public:
    using InputStream = io::InputStream;
    using OutputStream = io::OutputStream;

    /**
     * @brief 从流中接收一个请求
     *
     * @param source 目标源
     * @param request 请求
     * @return true 接收请求成功
     * @return false 接收请求失败
     */
    static bool recvRequest(InputStream *source, RequestHeader *request) noexcept;
    /**
     * @brief 向流中发送一个请求
     *
     * @param dest 目的流
     * @param request 请求
     * @return true 发送请求成功
     * @return false 发送请求失败
     */
    static bool sendRequest(OutputStream *dest, RequestHeader *request) noexcept;
    /**
     * @brief 从流中接收一个响应
     *
     * @param source 目标源
     * @param response 响应
     * @return true 接收响应成功
     * @return false 接收响应失败
     */
    static bool recvResponse(InputStream *source, ResponseHeader *response) noexcept;
    /**
     * @brief 向流中发送一个响应
     *
     * @param dest 目的流
     * @param response 响应
     * @return true 发送响应成功
     * @return false 发送响应失败
     */
    static bool sendResponse(OutputStream *dest, ResponseHeader *response) noexcept;

    static Cookie::Ptr parseFromSetCookie(const std::string &text) noexcept;

    static CookieMap::Ptr parseFromCookie(const std::string &text) noexcept;

    /// content-type 拓展名映射
    static std::map<std::string, std::string> content_type_map;

private:
    static bool getLine(InputStream *source, text::StringBuilder &builder) noexcept;

    inline static bool recvHeader(InputStream *source, text::StringBuilder &builder, Header *header, bool is_resp = false) noexcept;
    inline static bool sendHeader(OutputStream *dest, Header *header, bool is_resp = false) noexcept;

    inline static bool sendSetCookie(OutputStream *dest, const CookieMap::Ptr &cookies) noexcept;
    inline static bool sendCookie(OutputStream *dest, const CookieMap::Ptr &cookies) noexcept;
};
} // namespace sese::net::http