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
 * @file HttpUtil.h
 * @brief HTTP Utility Class
 * @author kaoru
 * @date May 17, 2022
 */

#pragma once

#include <sese/net/http/RequestHeader.h>
#include <sese/net/http/ResponseHeader.h>
#include <sese/text/StringBuilder.h>
#include <sese/io/Stream.h>

namespace sese::net::http {

/// @brief HTTP Utility Class
class HttpUtil {
public:
    using InputStream = io::InputStream;
    using OutputStream = io::OutputStream;

    /**
     * @brief Receive a request from a stream
     *
     * @param source Target source
     * @param request Request
     * @return true If the request is successfully received
     * @return false If the request fails to be received
     */
    static bool recvRequest(InputStream *source, RequestHeader *request) noexcept;
    /**
     * @brief Send a request to a stream
     *
     * @param dest Destination stream
     * @param request Request
     * @return true If the request is successfully sent
     * @return false If the request fails to be sent
     */
    static bool sendRequest(OutputStream *dest, RequestHeader *request) noexcept;
    /**
     * @brief Receive a response from a stream
     *
     * @param source Target source
     * @param response Response
     * @return true If the response is successfully received
     * @return false If the response fails to be received
     */
    static bool recvResponse(InputStream *source, ResponseHeader *response) noexcept;
    /**
     * @brief Send a response to a stream
     *
     * @param dest Destination stream
     * @param response Response
     * @return true If the response is successfully sent
     * @return false If the response fails to be sent
     */
    static bool sendResponse(OutputStream *dest, ResponseHeader *response) noexcept;

    static Cookie::Ptr parseFromSetCookie(const std::string &text) noexcept;

    static CookieMap::Ptr parseFromCookie(const std::string &text) noexcept;

    /// Content-Type extension mapping
    static std::map<std::string, std::string> content_type_map;

private:
    static bool getLine(InputStream *source, text::StringBuilder &builder) noexcept;

    inline static bool recvHeader(InputStream *source, text::StringBuilder &builder, Header *header, bool is_resp = false) noexcept;
    inline static bool sendHeader(OutputStream *dest, Header *header, bool is_resp = false) noexcept;

    inline static bool sendSetCookie(OutputStream *dest, const CookieMap::Ptr &cookies) noexcept;
    inline static bool sendCookie(OutputStream *dest, const CookieMap::Ptr &cookies) noexcept;
};
} // namespace sese::net::http
