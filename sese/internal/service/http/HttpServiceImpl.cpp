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

#include <sese/internal/service/http/HttpServiceImpl.h>
#include <sese/internal/net/AsioIPConvert.h>
#include <sese/internal/net/AsioSSLContextConvert.h>
#include <sese/net/http/HttpUtil.h>
#include <sese/text/DateTimeFormatter.h>
#include <sese/util/DateTime.h>
#include <sese/text/StringBuilder.h>
#include <sese/util/Util.h>

#include <sese/Log.h>

#include <filesystem>

sese::internal::service::http::HttpServiceImpl::HttpServiceImpl(
        const sese::net::IPAddress::Ptr &address,
        SSLContextPtr ssl_context,
        uint32_t keepalive,
        std::string &serv_name,
        MountPointMap &mount_points,
        ServletMap &servlets,
        FilterCallback &tail_filter,
        FilterMap &filters,
        ConnectionCallback &connection_callback
)
    : HttpService(address, std::move(ssl_context), keepalive, serv_name, mount_points, servlets, tail_filter, filters, connection_callback),
      io_context(),
      ssl_context(std::nullopt),
      acceptor(io_context) {
    thread = std::make_unique<Thread>(
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

bool sese::internal::service::http::HttpServiceImpl::startup() {
    auto addr = net::convert(address);
    auto endpoint = asio::ip::tcp::endpoint(addr, address->getPort());

    if (HttpService::ssl_context) {
        ssl_context = net::convert(std::move(HttpService::ssl_context));
    }

    if (ssl_context) {
        auto ctx = ssl_context->native_handle();
        // SSL_CTX_set_alpn_protos(ctx, alpn_protos, sizeof(alpn_protos));
        SSL_CTX_set_alpn_select_cb(ctx, alpnCallback, nullptr);
        SSL_CTX_set_mode(ctx, SSL_MODE_ENABLE_PARTIAL_WRITE);
    }

    error = acceptor.open(
            addr.is_v4()
                    ? asio::basic_socket_acceptor<asio::ip::tcp>::protocol_type::v4()
                    : asio::basic_socket_acceptor<asio::ip::tcp>::protocol_type::v6(),
            error
    );
    if (error)
        return false;

    error = acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true), error);
    if (error)
        return false;

    error = acceptor.bind(endpoint, error);
    if (error)
        return false;

    error = acceptor.listen(asio::socket_base::max_listen_connections, error);
    if (error)
        return false;

    thread->start();

    return true;
}

bool sese::internal::service::http::HttpServiceImpl::shutdown() {
    asio::post(acceptor.get_executor(), [this] {
        error = acceptor.close(error);
    });
    asio::post(io_context.get_executor(), [this] {
        io_context.stop();
    });
    sese::sleep(0s);
    connections.clear();
    connections2.clear();
    // Use asio::post to ensure the thread exits normally and determines whether it can be joined
    if (thread->joinable()) {
        thread->join();
    }
    return !error;
}

int sese::internal::service::http::HttpServiceImpl::getLastError() {
    return error.value();
}

std::string sese::internal::service::http::HttpServiceImpl::getLastErrorMessage(){
    return error.message();
}

void sese::internal::service::http::HttpServiceImpl::handleFilter(const Handleable::Ptr &conn) const {
    auto &&req = conn->request;
    auto &&resp = conn->response;
    for (auto &&[uri_prefix, callback]: filters) {
        if (text::StringBuilder::startsWith(req.getUri(), uri_prefix)) {
            if (callback(req, resp)) {
                conn->conn_type = ConnType::NONE;
            } else {
                conn->conn_type = ConnType::FILTER;
                break;
            }
        }
    }
}

void sese::internal::service::http::HttpServiceImpl::handleRequest(const Handleable::Ptr &conn) const {
    conn->stopwatch.stop();
    auto &&req = conn->request;
    auto &&resp = conn->response;
    std::filesystem::path filename;

    // Filter matching
    // for (auto &&[uri_prefix, callback]: filters) {
    //     if (text::StringBuilder::startsWith(req.getUri(), uri_prefix)) {
    //         conn->conn_type = ConnType::FILTER;
    //         if (callback(req, resp)) {
    //             conn->conn_type = ConnType::NONE;
    //         }
    //     }
    // }

    // Mount point matching
    if (conn->conn_type == ConnType::NONE) {
        for (auto &&[uri_prefix, mount_point]: mount_points) {
            if (text::StringBuilder::startsWith(req.getUri(), uri_prefix)) {
                conn->conn_type = ConnType::FILE_DOWNLOAD;
                filename = mount_point + "/" + req.getUri().substr(uri_prefix.length());
                // Confirm the file name and proceed to the next step
                break;
            }
        }
    }

    if (conn->conn_type == ConnType::NONE) {
        auto iterator = servlets.find(req.getUri());
        if (iterator == servlets.end()) {
            resp.setCode(404);
        } else {
            auto ctx = sese::net::http::HttpServletContext(req, resp, conn->remote_address);
            iterator->second.invoke(ctx);
            conn->conn_type = ConnType::CONTROLLER;
        }
        resp.set("content-length", std::to_string(resp.getBody().getLength()));
    } else if (conn->conn_type == ConnType::FILE_DOWNLOAD) {
        if (!exists(filename) ||
            !is_regular_file(filename) ||
            is_directory(filename)) {
            resp.setCode(404);
            resp.set("content-length", std::to_string(resp.getBody().getLength()));
            conn->conn_type = ConnType::NONE;
            goto uni_handle;
        }

        conn->file = io::File::create(filename.string(), io::File::B_READ);
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
            // No range file, manually set range
            conn->ranges.emplace_back(0, conn->filesize);
            conn->range_iterator = conn->ranges.begin();
            // Normal documents
            resp.set("content-length", std::to_string(conn->filesize));
            resp.setCode(200);
        } else if (conn->ranges.size() == 1) {
            // Single range file
            conn->range_iterator = conn->ranges.begin();
            // Check ranges
            if (conn->ranges[0].begin + conn->ranges[0].len > conn->filesize) {
                resp.setCode(416);
                resp.set("content-length", std::to_string(resp.getBody().getLength()));
                goto uni_handle;
            }
            // content-length
            resp.set("content-length", std::to_string(conn->ranges[0].len));
            resp.setCode(206);
        } else {
            // Multi-range file
            conn->range_iterator = conn->ranges.begin();
            size_t content_length = 0;
            // Validate ranges and calculate total length
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
        uint64_t time = to_time_t(last_modified) * 1000 * 1000;
        resp.set("last-modified", text::DateTimeFormatter::format(DateTime(time, 0), TIME_GREENWICH_MEAN_PATTERN));
    }

uni_handle:
    if (req.getVersion() == sese::net::http::HttpVersion::VERSION_1_1) {
        auto keepalive_str = req.get("connection", "close");
        conn->keepalive = strcmpDoNotCase(keepalive_str.c_str(), "keep-alive");

        if (conn->keepalive) {
            resp.set("connection", "keep-alive");
            resp.set("keep-alive", "timeout=" + std::to_string(keepalive));
        }
    }
    resp.set("server", this->serv_name);
    resp.set("accept-range", "bytes");
    if (tail_filter && (resp.getCode() != 200 && resp.getCode() != 201)) {
        if(tail_filter(req, resp)) {
            resp.set("content-length", std::to_string(resp.getBody().getReadableSize()));
            conn->conn_type = ConnType::CONTROLLER;
        }
    }
    SESE_INFO("{} {} {} in {}ms", sese::net::http::requestTypeToString(req.getType()), req.getUri(), resp.getCode(), conn->stopwatch.stop().getTotalMilliseconds());
}

void sese::internal::service::http::HttpServiceImpl::handleAccept() {
    auto accept_socket = std::make_shared<HttpConnectionImpl::Socket>(io_context);
    acceptor.async_accept(
            *accept_socket,
            [this, accept_socket](const asio::error_code &e) {
                if (!acceptor.is_open()) {
                    return;
                }
                if (e.value() == 0) {
                    auto remote_address = sese::internal::net::convert(accept_socket->remote_endpoint());
                    if (connection_callback && !connection_callback(remote_address)) {
                        this->handleAccept();
                        return;
                    }
                    auto conn = std::make_shared<HttpConnectionImpl>(
                            shared_from_this(),
                            io_context,
                            remote_address,
                            accept_socket
                    );
                    this->connections.emplace(conn);
                    conn->readHeader();
                    // conn->readMagic();
                }
                this->handleAccept();
            }
    );
}

int sese::internal::service::http::HttpServiceImpl::alpnCallback(
        SSL *ssl,
        const uint8_t **out,
        uint8_t *out_length,
        const uint8_t *in,
        uint32_t in_length,
        void *data
) {
    if (SSL_select_next_proto(
                const_cast<unsigned char **>(out),
                out_length,
                ALPN_PROTOS,
                sizeof(ALPN_PROTOS),
                in,
                in_length
        ) != OPENSSL_NPN_NEGOTIATED) {
        *out = nullptr;
        *out_length = 0;
        return SSL_TLSEXT_ERR_NOACK;
    }
    return SSL_TLSEXT_ERR_OK;
}

void sese::internal::service::http::HttpServiceImpl::handleSSLAccept() {
    auto accept_socket = std::make_shared<HttpConnectionImpl::Socket>(io_context);
    acceptor.async_accept(
            *accept_socket,
            [this, accept_socket](const asio::error_code &e) {
                if (!acceptor.is_open()) {
                    return;
                }
                if (e.value() == 0) {
                    auto remote_address = sese::internal::net::convert(accept_socket->remote_endpoint());
                    if (connection_callback && !connection_callback(remote_address)) {
                        this->handleSSLAccept();
                        return;
                    }
                    auto accept_stream = std::make_shared<HttpsConnectionImpl::Stream>(
                            std::move(*accept_socket), ssl_context.value()
                    );
                    accept_stream->async_handshake(
                            asio::ssl::stream_base::server,
                            [this, remote_address, accept_stream](const asio::error_code &e) {
                                if (e.value() == 0) {
                                    const uint8_t *data = nullptr;
                                    uint32_t data_length;
                                    SSL_get0_alpn_selected(accept_stream->native_handle(), &data, &data_length);
                                    auto proto = std::string_view(reinterpret_cast<const char *>(data), data_length);
                                    if (proto == "http/1.1") {
                                        // SESE_INFO("selected http/1.1");
                                        auto conn = std::make_shared<HttpsConnectionImpl>(
                                                shared_from_this(),
                                                io_context,
                                                remote_address,
                                                accept_stream
                                        );
                                        this->connections.emplace(conn);
                                        conn->readHeader();
                                    } else if (proto == "h2") {
                                        // SESE_INFO("selected http/2");
                                        auto conn = std::make_shared<HttpsConnectionExImpl>(
                                                shared_from_this(),
                                                io_context,
                                                remote_address,
                                                accept_stream
                                        );
                                        this->connections2.emplace(conn);
                                        conn->readMagic();
                                    } else {
                                        // SESE_WARN("unknown proto");
                                        auto conn = std::make_shared<HttpsConnectionImpl>(
                                                shared_from_this(),
                                                io_context,
                                                remote_address,
                                                accept_stream
                                        );
                                        this->connections.emplace(conn);
                                        conn->readHeader();
                                    }
                                }
                            }
                    );
                }
                this->handleSSLAccept();
            }
    );
}
