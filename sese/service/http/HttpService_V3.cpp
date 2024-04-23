#include <sese/service/http/HttpService_V3.h>
#include <sese/internal/net/AsioIPConvert.h>
#include <sese/internal/net/AsioSSLContextConvert.h>
#include <sese/net/http/HttpUtil.h>
#include <sese/io/FakeStream.h>
#include <sese/Util.h>

using namespace sese::service::http::v3;

inline auto toNumber(const std::string &str) {
    char *end;
    return static_cast<size_t>(std::strtol(str.c_str(), &end, 10));
}

HttpService::HttpService()
    : io_context(),
      acceptor(io_context) {
}

bool HttpService::startup() {
    auto addr = internal::net::convert(address);
    auto endpoint = asio::ip::tcp::endpoint(addr, address->getPort());
    error = acceptor.bind(endpoint, error);
    if (error) return false;

    error = acceptor.set_option(asio::socket_base::reuse_address(true), error);
    if (error) return false;

    error = acceptor.bind(endpoint, error);
    if (error) return false;

    error = acceptor.listen(asio::socket_base::max_listen_connections, error);
    if (error) return false;
    return true;
}

bool HttpService::shutdown() {
    error = acceptor.close(error);
    return !error;
}

int HttpService::getLastError() {
    return error.value();
}

void HttpConnection::readNextLine(const HttpConnection::Ptr &conn) {
    asio::async_read_until(conn->socket, conn->buffer_reader, "\r\n", [conn](const asio::error_code &error, std::size_t size) {
        if (size == 0) {
            auto std_input = std::istream{&conn->buffer_reader};
            auto stream = io::StdInputStreamWrapper(std_input);
            auto parse_status = net::http::HttpUtil::recvRequest(&stream, &conn->request);
            conn->buffer_reader.consume(conn->buffer_reader.size());
            conn->expect_length = toNumber(conn->request.get("content-length", "0"));
            conn->real_length = 0;
            readBody(conn);
        } else {
            readNextLine(conn);
        }
    });
}

void HttpConnection::readBody(const HttpConnection::Ptr &conn) {
    asio::async_read(conn->socket, conn->buffer_reader, [conn](asio::error_code &code, std::size_t bytes_transferred) {
        conn->real_length += bytes_transferred;
        if (conn->expect_length == conn->real_length) {
            handleRequest(conn);
        } else {
            readBody(conn);
        }
    });
}

void HttpConnection::handleRequest(const HttpConnection::Ptr &conn) {
    conn->service->handleRequest(conn);
    conn->response.set("content-length", std::to_string(conn->response.getBody().getLength()));
    auto std_output = std::ostream{&conn->buffer_writer};
    auto stream = io::StdOutputStreamWrapper(std_output);
    net::http::HttpUtil::sendResponse(&stream, &conn->response);

    conn->real_length = 0;
    conn->expect_length = conn->buffer_writer.size();
    writeHeader(conn);
}

void HttpConnection::writeHeader(const HttpConnection::Ptr &conn) {
    asio::async_write(conn->socket, conn->buffer_writer, [conn](asio::error_code &code, std::size_t bytes_transferred) {
        conn->real_length += bytes_transferred;
        if (conn->expect_length == conn->real_length) {
            conn->buffer_writer.consume(conn->buffer_writer.size());
            conn->expect_length = conn->response.getBody().getLength();
            conn->real_length = 0;

            auto std_output = std::ostream{&conn->buffer_writer};
            auto stream = io::StdOutputStreamWrapper(std_output);
            streamMove(&stream, &conn->response.getBody(), conn->expect_length);

            writeBody(conn);
        } else {
            writeHeader(conn);
        }
    });
}

void HttpConnection::writeBody(const HttpConnection::Ptr &conn) {
    asio::async_write(conn->socket, conn->buffer_writer, [conn](asio::error_code &code, std::size_t bytes_transferred) {
        conn->real_length += bytes_transferred;
        if (conn->expect_length == conn->real_length) {
            conn->buffer_writer.consume(conn->buffer_writer.size());
            conn->expect_length = 0;
            conn->real_length = 0;
            // done
        } else {
            writeBody(conn);
        }
    });
}
