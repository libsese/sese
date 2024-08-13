#pragma once

#include <asio.hpp>
#include <asio/ssl/stream.hpp>

#include <memory>

#include "ConnType.h"

class HttpServiceImpl;

struct HttpConnectionEx : std::enable_shared_from_this<HttpConnectionEx> {
    using Ptr = std::shared_ptr<HttpConnectionEx>;

    Ptr getPtr() { return shared_from_this(); } // NOLINT

    ConnType conn_type = ConnType::NONE;

    HttpConnectionEx(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &io_context);

    virtual ~HttpConnectionEx() = default;

    bool keepalive = false;
    asio::system_timer timer;

    std::weak_ptr<HttpServiceImpl> service;

    /// 写入块函数，此函数会确保写入完指定大小的缓存，出现意外则直接回调
    /// @param buffer 缓存指针
    /// @param length 缓存大小
    /// @param callback 完成回调函数
    virtual void writeBlock(const char *buffer, size_t length,
                            const std::function<void(const asio::error_code &code)> &callback) = 0;

    /// 读取块函数，此函数会确保读取完指定大小的缓存，出现意外则直接回调
    /// @param buffer 缓存指针
    /// @param length 缓存大小
    /// @param callback 完成回调函数
    virtual void readBlock(char *buffer, size_t length, const std::function<void(const asio::error_code &code)> &callback) = 0;
};

struct HttpConnectionExImpl final : HttpConnectionEx {
    using Ptr = std::shared_ptr<HttpConnectionExImpl>;
    using Socket = asio::ip::tcp::socket;
    using SharedSocket = std::shared_ptr<Socket>;

    Ptr getPtr() { return std::reinterpret_pointer_cast<HttpConnectionExImpl>(shared_from_this()); } // NOLINT

    SharedSocket socket;

    HttpConnectionExImpl(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &context,
                       SharedSocket socket);

    void writeBlock(const char *buffer, size_t length,
                    const std::function<void(const asio::error_code &code)> &callback) override;

    void readBlock(char *buffer, size_t length, const std::function<void(const asio::error_code &code)> &callback) override;
};

struct HttpsConnectionExImpl final : HttpConnectionEx {
    using Ptr = std::shared_ptr<HttpsConnectionExImpl>;
    using Stream = asio::ssl::stream<asio::ip::tcp::socket>;
    using SharedStream = std::shared_ptr<Stream>;

    Ptr getPtr() { return std::reinterpret_pointer_cast<HttpsConnectionExImpl>(shared_from_this()); } // NOLINT

    SharedStream stream;

    HttpsConnectionExImpl(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &context, SharedStream stream);

    void writeBlock(const char *buffer, size_t length,
                    const std::function<void(const asio::error_code &code)> &callback) override;

    void readBlock(char *buffer, size_t length, const std::function<void(const asio::error_code &code)> &callback) override;
};
