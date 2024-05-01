#include <sese/internal/service/http/HttpServiceImpl_V3.h>
#include <sese/internal/net/AsioIPConvert.h>
#include <sese/internal/net/AsioSSLContextConvert.h>
#include <sese/net/http/HttpUtil.h>
#include <sese/io/FakeStream.h>
#include <sese/text/DateTimeFormatter.h>
#include <sese/text/StringBuilder.h>
#include <sese/util/Util.h>
#include <sese/record/Marco.h>

#include <filesystem>

using namespace sese::internal::service::http::v3;

HttpServiceImpl::HttpServiceImpl(
        const sese::net::IPAddress::Ptr &address,
        const security::SSLContext::Ptr &ssl_context,
        uint32_t keepalive,
        std::string &serv_name,
        MountPointMap &mount_points,
        ServletMap &servlets
) : HttpService(address, ssl_context, keepalive, serv_name, mount_points, servlets),
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
            "HttpServiveAcceptor"
    );
}

bool HttpServiceImpl::startup() {
    auto addr = net::convert(address);
    auto endpoint = asio::ip::tcp::endpoint(addr, address->getPort());

    if (HttpService::ssl_context) {
        ssl_context = net::convert(HttpService::ssl_context);
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

void HttpServiceImpl::handleRequest(const HttpConnection::Ptr &conn) {
    auto &&req = conn->request;
    auto &&resp = conn->response;

    // 挂载点匹配
    std::filesystem::path filename;
    for (auto &&mount_point: mount_points) {
        if (text::StringBuilder::startsWith(req.getUri(), mount_point.first)) {
            conn->conn_type = HttpConnection::ConnType::FILE_DOWNLOAD;
            filename = mount_point.second + req.getUri().substr(mount_point.first.length());
        }
    }

    if (conn->conn_type == HttpConnection::ConnType::NORMAL) {
        auto iterator = servlets.find(req.getUri());
        if (iterator == servlets.end()) {
            resp.setCode(404);
        } else {
            iterator->second.invoke(req, resp);
        }
        resp.set("content-length", std::to_string(resp.getBody().getLength()));
    } else if (conn->conn_type == HttpConnection::ConnType::FILE_DOWNLOAD) {
        if (!exists(filename) ||
            !is_regular_file(filename) ||
            is_directory(filename)) {
            resp.setCode(404);
            resp.set("content-length", std::to_string(resp.getBody().getLength()));
            goto uni_handle;
        }

        conn->file = io::File::create(filename.string(), BINARY_READ_EXISTED);
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
        uint64_t time = to_time_t(last_modified) * 1000 * 1000;
        resp.set("last-modified", text::DateTimeFormatter::format(DateTime(time, 0), TIME_GREENWICH_MEAN_PATTERN));
    }

uni_handle:
    auto keepalive_str = req.get("connection", "close");
    conn->keepalive = strcmpDoNotCase(keepalive_str.c_str(), "keep-alive");

    if (conn->keepalive) {
        resp.set("connection", "keep-alive");
        resp.set("keep-alive", "timeout=" + std::to_string(keepalive));
    }
    resp.set("server", this->serv_name);
    resp.set("accept-range", "bytes");
}

void HttpServiceImpl::handleAccept() {
    auto conn = std::make_shared<HttpConnectionImpl>(shared_from_this(), io_context);
    acceptor.async_accept(
            conn->socket,
            [this, conn](const asio::error_code &error) {
                if (error) return;
                this->connections.emplace(conn);
                conn->readHeader();
                this->handleAccept();
            }
    );
}

void HttpServiceImpl::handleSSLAccept() {
    auto conn = std::make_shared<HttpSSLConnectionImpl>(shared_from_this(), io_context);
    acceptor.async_accept(
            conn->socket,
            [this, conn](const asio::error_code &error) {
                if (error) {
                    return;
                }
                conn->stream = std::make_unique<asio::ssl::stream<asio::ip::tcp::socket &>>(conn->socket, ssl_context.value());
                conn->stream->async_handshake(
                        asio::ssl::stream_base::server,
                        [conn, this](const asio::error_code &error) {
                            if (error) return;
                            this->connections.emplace(conn);
                            conn->readHeader();
                        }
                );
                this->handleSSLAccept();
            }
    );
}

HttpConnection::HttpConnection(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &context)
    : socket(context),
      timer(context, asio::chrono::seconds{service->getKeepalive()}),
      expect_length(0),
      real_length(0),
      service(service) {
}

void HttpConnection::writeSingleRange() {
    auto l = std::min<size_t>(this->expect_length - this->real_length, MTU_VALUE);
    this->real_length += l;
    l = this->file->read(this->send_buffer, l);
    this->writeBlock(this->send_buffer, l, [conn = shared_from_this()](const asio::error_code &error) {
        if (error) {
            conn->disponse();
            return;
        }
        if (conn->expect_length > conn->real_length) {
            conn->writeSingleRange();
        } else {
            // keepalive
            conn->checkKeepalive();
        }
    });
}

void HttpConnection::writeRanges() {
    if (this->real_length >= this->expect_length) {
        if (this->range_iterator == this->ranges.begin()) {
            // 首个区间
            auto subheader = std::string("--") + HTTPD_BOUNDARY + "\r\n" +
                             "content-type: " + this->content_type + "\r\n" +
                             "content-range: " + this->range_iterator->toString(this->filesize) + "\r\n\r\n";
            assert(subheader.length() <= MTU_VALUE);
            std::memcpy(this->send_buffer, subheader.data(), subheader.length());
            this->writeBlock(this->send_buffer, subheader.length(), [conn = shared_from_this()](const asio::error_code &error) {
                if (error) {
                    return;
                }
                conn->file->setSeek(static_cast<int64_t>(conn->range_iterator->begin), io::Seek::BEGIN);
                conn->expect_length = conn->range_iterator->len;
                conn->real_length = 0;
                conn->writeRanges();
                conn->range_iterator += 1;
            });
            return;
        }
        if (this->range_iterator == this->ranges.end()) {
            // 最后一个区间
            auto subheader = std::string("\r\n--") + HTTPD_BOUNDARY + "--\r\n";
            std::memcpy(this->send_buffer, subheader.data(), subheader.length());
            assert(subheader.length() <= MTU_VALUE);
            this->writeBlock(this->send_buffer, subheader.length(), [conn = shared_from_this()](const asio::error_code &error) {
                if (error) {
                    return;
                }
                conn->checkKeepalive();
            });
        } else {
            // 中间的区间
            auto subheader = std::string("\r\n--") + HTTPD_BOUNDARY + "\r\n" +
                             "content-type: " + this->content_type + "\r\n" +
                             "content-range: " + this->range_iterator->toString(this->filesize) + "\r\n\r\n";
            assert(subheader.length() <= MTU_VALUE);
            std::memcpy(this->send_buffer, subheader.data(), subheader.length());
            this->writeBlock(this->send_buffer, subheader.length(), [conn = shared_from_this()](const asio::error_code &error) {
                if (error) {
                    return;
                }
                conn->file->setSeek(static_cast<int64_t>(conn->range_iterator->begin), io::Seek::BEGIN);
                conn->expect_length = conn->range_iterator->len;
                conn->real_length = 0;
                conn->writeRanges();
                conn->range_iterator += 1;
            });
        }
    } else {
        auto l = std::min<size_t>(this->expect_length - this->real_length, MTU_VALUE);
        this->real_length += l;
        l = this->file->read(this->send_buffer, l);
        this->writeBlock(this->send_buffer, l, [conn = shared_from_this()](const asio::error_code &error) {
            if (error) {
                conn->disponse();
                return;
            }
            conn->writeRanges();
        });
    }
}

void HttpConnection::disponse() {
    service->connections.erase(shared_from_this());
}

void HttpConnection::reset() {
    conn_type = ConnType::NORMAL;

    expect_length = 0;
    real_length = 0;
    ranges.clear();

    request.clear();
    request.queryArgsClear();
    request.getBody().freeCapacity();
    if (auto cookies = request.getCookies()) cookies->clear();

    response.setCode(200);
    response.clear();
    response.getBody().freeCapacity();
    if (auto cookies = response.getCookies()) cookies->clear();
}
