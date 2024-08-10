#pragma once

#include <asio.hpp>
#include <asio/ssl/stream.hpp>

#include <sese/service/iocp/IOBuf.h>
#include <sese/net/http/Range.h>
#include <sese/io/File.h>

class HttpServiceImpl;

/// Http 普通连接实现
struct HttpConnection : std::enable_shared_from_this<HttpConnection> {
    using Ptr = std::shared_ptr<HttpConnection>;

    Ptr getPtr() { return shared_from_this(); }

    enum class ConnType {
        FILTER,
        FILE_DOWNLOAD,
        NORMAL
    } conn_type = ConnType::NORMAL;

    HttpConnection(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &io_context);

    virtual ~HttpConnection() = default;

    sese::net::http::Request request;
    sese::net::http::Response response;
    asio::ip::tcp::socket socket;

    bool keepalive = false;
    asio::system_timer timer;

    void reset();

    std::string content_type = "application/x-";
    size_t filesize = 0;
    char send_buffer[MTU_VALUE]{};
    size_t expect_length;
    size_t real_length;
    sese::io::File::Ptr file;
    std::vector<sese::net::http::Range> ranges;
    std::vector<sese::net::http::Range>::iterator range_iterator = ranges.begin();

    bool is0x0a = false;
    sese::iocp::IOBuf io_buffer;
    std::unique_ptr<sese::iocp::IOBufNode> node;
    sese::io::ByteBuilder dynamic_buffer;

    std::weak_ptr<HttpServiceImpl> service;

    void readHeader();

    void readBody();

    void handleRequest();

    void writeHeader();

    void writeBody();

    /// 写入块函数，此函数会确保写完所有的缓存，出现意外则连接断开
    /// @note 此函数必须实现
    /// @param buffer 缓存指针
    /// @param length 缓存大小
    /// @param callback 完成回调函数
    virtual void writeBlock(const char *buffer, size_t length,
                            const std::function<void(const asio::error_code &code)> &callback);

    /// 读取函数，此函数会调用对应的 asio::async_read_some
    /// @param buffer asio::buffer
    /// @param callback 回调函数
    virtual void asyncReadSome(const asio::mutable_buffers_1 &buffer,
                               const std::function<void(const asio::error_code &error, std::size_t bytes_transferred)> &
                               callback);

    /// 当一个请求处理完成后被调用，用于判断是否断开当前连接
    /// @note 此函数必须被实现
    virtual void checkKeepalive();

    /// 连接被彻底释放前调用，用于做一些成员变量的收尾工作
    /// @note 此函数是可选实现的
    virtual void disponse();

    void writeSingleRange();

    void writeRanges();
};

/// Http SSL 连接实现
struct HttpsConnection final : HttpConnection {
    using Ptr = std::shared_ptr<HttpsConnection>;

    HttpsConnection(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &context);

    ~HttpsConnection() override;

    std::unique_ptr<asio::ssl::stream<asio::ip::tcp::socket &> > stream;

    void writeBlock(const char *buffer, size_t length,
                    const std::function<void(const asio::error_code &code)> &callback) override;

    void asyncReadSome(const asio::mutable_buffers_1 &buffer, const std::function<void(const asio::error_code &error, std::size_t bytes_transferred)> &callback) override;

    void checkKeepalive() override;
};