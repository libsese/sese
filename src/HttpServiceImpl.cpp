#include "HttpServiceImpl.h"

#include <sese/internal/service/http/HttpServiceImpl_V3.h>
#include <sese/internal/net/AsioIPConvert.h>
#include <sese/internal/net/AsioSSLContextConvert.h>
#include <sese/net/http/HttpUtil.h>
#include <sese/text/DateTimeFormatter.h>
#include <sese/util/DateTime.h>
#include <sese/text/StringBuilder.h>
#include <sese/util/Util.h>

#include <filesystem>

HttpServiceImpl::HttpServiceImpl(
    const sese::net::IPAddress::Ptr &address,
    SSLContextPtr ssl_context,
    uint32_t keepalive,
    std::string &serv_name,
    MountPointMap &mount_points,
    ServletMap &servlets,
    FilterMap &filters
) : HttpService(address, std::move(ssl_context), keepalive, serv_name, mount_points, servlets, filters),
    io_context(),
    ssl_context(std::nullopt),
    acceptor(io_context) {
    thread = std::make_unique<sese::Thread>(
        [this] {
            if (this->ssl_context.has_value()) {
                this->handleSSLAccept();
            } else {
                this->handleAccept();
            }
            this->io_context.run();
        },
        "HttpServiceAcceptor"
    );
}

bool HttpServiceImpl::startup() {
    auto addr = sese::internal::net::convert(address);
    auto endpoint = asio::ip::tcp::endpoint(addr, address->getPort());

    if (HttpService::ssl_context) {
        ssl_context = sese::internal::net::convert(std::move(HttpService::ssl_context));
    }

    if (ssl_context) {
        auto ctx = ssl_context->native_handle();
        // SSL_CTX_set_alpn_protos(ctx, alpn_protos, sizeof(alpn_protos));
        SSL_CTX_set_alpn_select_cb(ctx, alpnCallback, nullptr);
    }

    error = acceptor.open(
        addr.is_v4()
            ? asio::basic_socket_acceptor<asio::ip::tcp>::protocol_type::v4()
            : asio::basic_socket_acceptor<asio::ip::tcp>::protocol_type::v6(),
        error
    );
    if (error) return false;

    error = acceptor.set_option(asio::socket_base::reuse_address(true), error);
    if (error) return false;

    error = acceptor.bind(endpoint, error);
    if (error) return false;

    error = acceptor.listen(asio::socket_base::max_listen_connections, error);
    if (error) return false;

    thread->start();

    return true;
}

bool HttpServiceImpl::shutdown() {
    error = acceptor.close(error);
    io_context.stop();
    thread->join();
    return !error;
}

int HttpServiceImpl::getLastError() {
    return error.value();
}

void HttpServiceImpl::handleRequest(const HttpConnection::Ptr &conn) const {
    auto &&req = conn->request;
    auto &&resp = conn->response;
    std::filesystem::path filename;

    // 过滤器匹配
    for (auto &&[uri_prefix, callback]: filters) {
        if (sese::text::StringBuilder::startsWith(req.getUri(), uri_prefix)) {
            conn->conn_type = ConnType::FILTER;
            if (callback(req, resp)) {
                conn->conn_type = ConnType::NONE;
            }
        }
    }

    // 挂载点匹配
    if (conn->conn_type == ConnType::NONE) {
        for (auto &&[uri_prefix, mount_point]: mount_points) {
            if (sese::text::StringBuilder::startsWith(req.getUri(), uri_prefix)) {
                conn->conn_type = ConnType::FILE_DOWNLOAD;
                filename = mount_point + req.getUri().substr(uri_prefix.length());
                // 确认文件名后进行下一步操作
                break;
            }
        }
    }

    if (conn->conn_type == ConnType::NONE) {
        auto iterator = servlets.find(req.getUri());
        if (iterator == servlets.end()) {
            resp.setCode(404);
        } else {
            iterator->second.invoke(req, resp);
            conn->conn_type = ConnType::CONTROLLER;
        }
        resp.set("content-length", std::to_string(resp.getBody().getLength()));
    } else if (conn->conn_type == ConnType::FILE_DOWNLOAD) {
        if (!exists(filename) ||
            !is_regular_file(filename) ||
            is_directory(filename)) {
            resp.setCode(404);
            resp.set("content-length", std::to_string(resp.getBody().getLength()));
            goto uni_handle;
        }

        conn->file = sese::io::File::create(filename.string(), sese::io::File::B_READ);
        if (!conn->file) {
            resp.setCode(500);
            resp.set("content-length", std::to_string(resp.getBody().getLength()));
            goto uni_handle;
        }

        if (filename.has_extension()) {
            auto ext = filename.extension().string().substr(1);
            auto type = sese::net::http::HttpUtil::content_type_map.find(ext);
            if (type == sese::net::http::HttpUtil::content_type_map.end()) {
                resp.set("content-type", conn->content_type);
            } else {
                resp.set("content-type", type->second);
                conn->content_type = type->second;
            }
        }

        conn->filesize = file_size(filename);
        conn->ranges = sese::net::http::Range::parse(req.get("Range", ""), conn->filesize);
        if (conn->ranges.empty()) {
            // 无区间文件，手动设置区间
            conn->ranges.emplace_back(0, conn->filesize);
            conn->range_iterator = conn->ranges.begin();
            // 普通文件
            resp.set("content-length", std::to_string(conn->filesize));
            resp.setCode(200);
        } else if (conn->ranges.size() == 1) {
            // 单区间文件
            conn->range_iterator = conn->ranges.begin();
            // 校验区间
            if (conn->ranges[0].begin + conn->ranges[0].len > conn->filesize) {
                resp.setCode(416);
                resp.set("content-length", std::to_string(resp.getBody().getLength()));
                goto uni_handle;
            }
            // content-length
            resp.set("content-length", std::to_string(conn->ranges[0].len));
            resp.setCode(206);
        } else {
            // 多区间文件
            conn->range_iterator = conn->ranges.begin();
            size_t content_length = 0;
            // 校验区间并计算总长度
            for (auto &&item: conn->ranges) {
                if (item.begin + item.len > conn->filesize) {
                    resp.setCode(416);
                    resp.set("content-length", std::to_string(resp.getBody().getLength()));
                    goto uni_handle;
                }
                content_length += 12 +
                        strlen(HTTPD_BOUNDARY) +
                        strlen("Content-Type: ") +
                        conn->content_type.length() +
                        strlen("Content-Range: ") +
                        item.toStringLength(conn->filesize) +
                        item.len;
            }
            content_length += 6 + strlen(HTTPD_BOUNDARY);
            // content-type
            resp.set("content-type", std::string("multipart/byteranges; boundary=") + HTTPD_BOUNDARY);
            // content-length
            resp.set("content-length", std::to_string(content_length));
            resp.setCode(206);
        }

        auto last_modified = last_write_time(filename);
        uint64_t time = sese::to_time_t(last_modified) * 1000 * 1000;
        resp.set("last-modified",
                 sese::text::DateTimeFormatter::format(sese::DateTime(time, 0), TIME_GREENWICH_MEAN_PATTERN));
    }

uni_handle:
    auto keepalive_str = req.get("connection", "close");
    conn->keepalive = sese::strcmpDoNotCase(keepalive_str.c_str(), "keep-alive");

    if (conn->keepalive) {
        resp.set("connection", "keep-alive");
        resp.set("keep-alive", "timeout=" + std::to_string(keepalive));
    }
    resp.set("server", this->serv_name);
    resp.set("accept-range", "bytes");
}

void HttpServiceImpl::handleAccept() {
    auto accept_socket = std::make_shared<HttpConnectionImpl::Socket>(io_context);
    acceptor.async_accept(
        *accept_socket,
        [this, accept_socket](const asio::error_code &e) {
            if (e.value() == 0) {
                auto conn = std::make_shared<HttpConnectionImpl>(
                    shared_from_this(),
                    io_context,
                    accept_socket
                );
                this->connections.emplace(conn);
                conn->readHeader();
            }
            this->handleAccept();
        }
    );
}

#include <sese/Log.h>

int HttpServiceImpl::alpnCallback(
    SSL *ssl,
    const uint8_t **out,
    uint8_t *out_length,
    const uint8_t *in,
    uint32_t in_length,
    void *data) {
    if (SSL_select_next_proto(
            const_cast<unsigned char **>(out),
            out_length,
            alpn_protos,
            sizeof(alpn_protos),
            in,
            in_length
        ) != OPENSSL_NPN_NEGOTIATED) {
        *out = nullptr;
        *out_length = 0;
        return SSL_TLSEXT_ERR_NOACK;
    }
    return SSL_TLSEXT_ERR_OK;
}


void HttpServiceImpl::handleSSLAccept() {
    auto accept_socket = std::make_shared<HttpConnectionImpl::Socket>(io_context);
    acceptor.async_accept(
        *accept_socket,
        [this, accept_socket](const asio::error_code &e) {
            if (e.value() == 0) {
                auto accept_stream = std::make_shared<HttpsConnectionImpl::Stream>(
                    std::move(*accept_socket), ssl_context.value());
                accept_stream->async_handshake(
                    asio::ssl::stream_base::server,
                    [this, accept_stream](const asio::error_code &e) {
                        if (e.value() == 0) {
                            const uint8_t *data = nullptr;
                            uint32_t dataLength;
                            SSL_get0_alpn_selected(accept_stream->native_handle(), &data, &dataLength);
                            auto proto = std::string_view(reinterpret_cast<const char *>(data), dataLength);
                            if (proto == "http/1.1") {
                                SESE_INFO("selected http/1.1");
                                auto conn = std::make_shared<HttpsConnectionImpl>(
                                    shared_from_this(),
                                    io_context,
                                    accept_stream
                                );
                                this->connections.emplace(conn);
                                conn->readHeader();
                            } else if (proto == "h2") {
                                SESE_INFO("selected http/2");
                            } else {
                                SESE_WARN("unknown proto");
                            }
                        }
                    });
            }
            this->handleSSLAccept();
        }
    );
}
