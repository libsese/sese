/// @file HttpServiceImpl_V3.h
/// @brief Http v3 服务实现
/// @author kaoru
/// @date 2024年05月1日

#pragma once

#include <sese/service/http/HttpService_V3.h>
#include <sese/service/iocp/IOBuf.h>
#include <sese/net/http/Range.h>
#include <sese/io/ByteBuilder.h>
#include <sese/io/File.h>

#include <asio.hpp>
#include <asio/ssl/stream.hpp>

#include <optional>
#include <set>

namespace sese::internal::service::http::v3 {

struct HttpConnection;
struct HttpConnectionImpl;
struct HttpSSLConnectionImpl;
class HttpServiceImpl;

/// Http 连接基类
struct HttpConnection : public std::enable_shared_from_this<HttpConnection> {
    using Ptr = std::shared_ptr<HttpConnection>;

    enum class ConnType {
        /// 表示请求被过滤器拦截
        FILTER,
        /// 表示请求是挂载点静态资源请求
        FILE_DOWNLOAD,
        /// 普通请求
        NORMAL
    } conn_type = ConnType::NORMAL;

    HttpConnection(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &context);
    virtual ~HttpConnection() = default;

    net::http::Request request;
    net::http::Response response;
    asio::ip::tcp::socket socket;

    bool keepalive = false;
    asio::system_timer timer;
    void reset();

    std::string content_type = "application/x-";
    size_t filesize = 0;
    char send_buffer[MTU_VALUE]{};
    size_t expect_length;
    size_t real_length;
    io::File::Ptr file;
    std::vector<net::http::Range> ranges;
    std::vector<net::http::Range>::iterator range_iterator = ranges.begin();

    std::weak_ptr<HttpServiceImpl> service;

    /// 写入块函数，此函数会确保写完所有的缓存，出现意外则连接断开
    /// @note 此函数必须实现
    /// @param buffer 缓存指针
    /// @param length 缓存大小
    /// @param callback 完成回调函数
    virtual void writeBlock(const char *buffer, size_t length, const std::function<void(const asio::error_code &code)> &callback) = 0;

    /// 当一个请求处理完成后被调用，用于判断是否断开当前连接
    /// @note 此函数必须被实现
    virtual void checkKeepalive() = 0;

    /// 连接被彻底释放前调用，用于做一些成员变量的收尾工作
    /// @note 此函数是可选实现的
    virtual void disponse();

    void writeSingleRange();
    void writeRanges();
};

/// Http 普通连接实现
struct HttpConnectionImpl final : HttpConnection {
    using Ptr = std::shared_ptr<HttpConnectionImpl>;
    Ptr getPtr() { return std::static_pointer_cast<HttpConnectionImpl>(shared_from_this()); }

    HttpConnectionImpl(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &context);

    io::ByteBuilder parse_buffer;
    asio::streambuf asio_dynamic_buffer;

    void writeBlock(const char *buffer, size_t length, const std::function<void(const asio::error_code &code)> &callback) override;

    void readHeader();
    void readBody();
    void handleRequest();
    void writeHeader();
    void writeBody();
    void checkKeepalive() override;
};

/// Http SSL 连接实现
struct HttpSSLConnectionImpl final : HttpConnection {
    using Ptr = std::shared_ptr<HttpSSLConnectionImpl>;
    Ptr getPtr() { return std::static_pointer_cast<HttpSSLConnectionImpl>(shared_from_this()); }

    HttpSSLConnectionImpl(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &context);
    ~HttpSSLConnectionImpl() override;

    std::unique_ptr<asio::ssl::stream<asio::ip::tcp::socket &>> stream;
    bool is0x0a = false;
    iocp::IOBuf io_buffer;
    std::unique_ptr<iocp::IOBufNode> node;
    io::ByteBuilder dynamic_buffer;

    void writeBlock(const char *buffer, size_t length, const std::function<void(const asio::error_code &code)> &callback) override;

    void readHeader();
    void readBody();
    void handleRequest();
    void writeHeader();
    void writeBody();
    void checkKeepalive() override;
};

/// Http 服务实现
class HttpServiceImpl final : public sese::service::http::v3::HttpService, public std::enable_shared_from_this<HttpServiceImpl> {
public:
    friend struct HttpConnection;

    HttpServiceImpl(
            const net::IPAddress::Ptr &address,
            SSLContextPtr ssl_context,
            uint32_t keepalive,
            std::string &serv_name,
            MountPointMap &mount_points,
            ServletMap &servlets,
            FilterMap &filters
    );

    bool startup() override;
    bool shutdown() override;
    int getLastError() override;
    uint32_t getKeepalive() const { return keepalive; }

    void handleRequest(const HttpConnection::Ptr &conn);

private:
    asio::io_context io_context;
    std::optional<asio::ssl::context> ssl_context;
    asio::ip::tcp::acceptor acceptor;
    asio::error_code error;

    void handleAccept();
    void handleSSLAccept();

    std::set<HttpConnection::Ptr> connections;
};

} // namespace sese::internal::service::http::v3