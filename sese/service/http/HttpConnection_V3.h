#pragma once

#include <sese/io/ByteBuilder.h>

namespace sese::service::http::v3 {
struct HttpConnection {
    const size_t BUFFER_READ_SIZE = 8192;
    const size_t BUFFER_WRITE_SIZE = 8192;

    int fd;
    void *ssl;
    io::ByteBuilder buffer_read{BUFFER_READ_SIZE, BUFFER_READ_SIZE};
    io::ByteBuilder buffer_write{BUFFER_WRITE_SIZE, BUFFER_WRITE_SIZE};
};
}