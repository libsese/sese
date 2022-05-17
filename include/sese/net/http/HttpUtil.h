/**
 * @file HttpUtil.h
 * @brief Http 工具类
 * @author kaoru
 * @date 2022年5月17日
 */
#pragma once
#include <sese/net/http/RequestHeader.h>
#include <sese/net/http/ResponseHeader.h>
#include <sese/Stream.h>
#include <sese/StringBuilder.h>

namespace sese::http {

    /// @brief Http 工具类
    class API HttpUtil {
    public:
        /**
         * @brief 从流中接收一个请求
         * 
         * @param source 目标源
         * @param request 请求
         * @return true 接收请求成功
         * @return false 接收请求失败
         */
        static bool recvRequest(const Stream::Ptr &source, const RequestHeader::Ptr &request) noexcept;
        /**
         * @brief 向流中发送一个请求
         * 
         * @param dest 目的流
         * @param request 请求
         * @return true 发送请求成功
         * @return false 发送请求失败
         */
        static bool sendRequest(const Stream::Ptr &dest, const RequestHeader::Ptr &request) noexcept;

        /**
         * @brief 从流中接收一个响应
         * 
         * @param source 目标源
         * @param response 响应
         * @return true 接收响应成功
         * @return false 接收响应失败
         */
        static bool recvResponse(const Stream::Ptr &source, const ResponseHeader::Ptr &response) noexcept;
        /**
         * @brief 向流中发送一个响应
         * 
         * @param dest 目的流
         * @param response 响应
         * @return true 发送响应成功
         * @return false 发送响应失败
         */
        static bool sendResponse(const Stream::Ptr &dest, const ResponseHeader::Ptr &response) noexcept;

    private:
        static bool getLine(const Stream::Ptr &source, StringBuilder &builder) noexcept;

        inline static bool recvHeader(const Stream::Ptr &source, StringBuilder &builder, const Header::Ptr &header) noexcept;
        inline static bool sendHeader(const Stream::Ptr &dest, const Header::Ptr &header) noexcept;
    };
}