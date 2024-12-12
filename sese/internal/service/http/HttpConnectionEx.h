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

#pragma once

#include <asio.hpp>
#include <asio/ssl/stream.hpp>


#include <sese/Config.h>
#include <sese/net/http/DynamicTable.h>
#include <sese/net/http/Http2Frame.h>
#include <sese/internal/service/http/Handleable.h>
#include <sese/net/IPv6Address.h>

#include <memory>
#include <queue>
#include <set>


namespace sese::internal::service::http {
class HttpServiceImpl;

struct HttpStream : Handleable {
    using Ptr = std::shared_ptr<HttpStream>;

    explicit HttpStream(uint32_t id, uint32_t write_window_size, const sese::net::IPAddress::Ptr &addr) noexcept;

    /// Initialize the current file compartment and iterate over the iterator
    void prepareRange();

    uint32_t id;
    /// Write to the peer window size
    uint32_t endpoint_window_size;
    /// Local read window
    uint32_t window_size = 0;
    uint16_t continue_type = 0;
    bool end_headers = false;
    bool end_stream = false;
    bool do_response = false;

    size_t expect_length;
    size_t real_length;

    sese::io::ByteBuilder temp_buffer;

    sese::net::http::Http2FrameInfo frame{};
};

struct HttpConnectionEx : std::enable_shared_from_this<HttpConnectionEx> {
    using Ptr = std::shared_ptr<HttpConnectionEx>;

    Ptr getPtr() { return shared_from_this(); } // NOLINT

    HttpConnectionEx(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &io_context, const sese::net::IPAddress::Ptr &addr);

    virtual ~HttpConnectionEx() = default;

    bool keepalive = false;
    asio::system_timer timer;

    sese::net::IPAddress::Ptr remote_address;

    std::weak_ptr<HttpServiceImpl> service;

    bool is_read = false;
    bool is_write = false;
    bool expect_ack = false;
    uint32_t accept_stream_count = 0;
    uint32_t latest_stream_ident = 0;

    // The maximum local frame size
    static constexpr uint32_t MAX_FRAME_SIZE = 16384;
    // Local initial window value
    static constexpr uint32_t INIT_WINDOW_SIZE = 65535;
    // Default dynamic table size
    static constexpr uint32_t HEADER_TABLE_SIZE = 8192;
    // The size of a single connection concurrency
    static constexpr uint32_t MAX_CONCURRENT_STREAMS = 16;

    sese::net::http::Http2FrameInfo frame{};
    // Temporary cache, which is used to read the initial connection magic number and frame header,
    // and take the maximum frame size
    char temp_buffer[MAX_FRAME_SIZE]{};
    uint32_t header_table_size = 4096;
    uint32_t enable_push = 0;
    uint32_t max_concurrent_stream = 0;
    // The value of the initial window on the peer
    uint32_t endpoint_init_window_size = 65535;
    // Write to the peer window size
    uint32_t endpoint_window_size = 65535;
    // The size of the local read window
    uint32_t window_size = 65535;
    // The maximum size of the peer frame
    uint32_t endpoint_max_frame_size = 16384;
    // The frame size used
    uint32_t max_frame_size = 16384;
    uint32_t max_header_list_size = 0;
    sese::net::http::DynamicTable req_dynamic_table;
    sese::net::http::DynamicTable resp_dynamic_table;
    std::map<uint32_t, HttpStream::Ptr> streams;
    std::set<uint32_t> closed_streams;

    /// Send queues
    std::vector<sese::net::http::Http2Frame::Ptr> pre_vector;
    std::vector<sese::net::http::Http2Frame::Ptr> vector;
    std::vector<asio::const_buffer> asio_buffers;

    /// Close stream
    /// @param id Stream ID
    void close(uint32_t id);

    virtual void checkKeepalive() = 0;

    void disponse();

    /// Write block function. This function ensures that the specified buffer is completely written,
    /// and calls back immediately if an error occurs
    /// @param buffers Buffer
    /// @param callback Completion callback function
    virtual void writeBlocks(const std::vector<asio::const_buffer> &buffers,
                             const std::function<void(const asio::error_code &code)> &callback) = 0;

    /// Write block function. This function ensures that a single block of buffer is completely written,
    /// and calls back immediately if an error occurs
    /// @param buffer Single block buffer
    /// @param size Size of the buffer
    /// @param callback Completion callback function
    virtual void writeBlock(const void *buffer, size_t size,
                            const std::function<void(const asio::error_code &code)> &callback) = 0;

    /// Read block function. This function ensures that the specified size of the buffer is completely read,
    /// and calls back immediately if an error occurs
    /// @param buffer Pointer to the buffer
    /// @param length Size of the buffer
    /// @param callback Completion callback function
    virtual void readBlock(char *buffer, size_t length,
                           const std::function<void(const asio::error_code &code)> &callback) = 0;

    void readMagic();

    void readFrameHeader();

    void handleFrameHeader();

    uint8_t handleSettingsFrame();

    void handleWindowUpdate();

    void handleRstStreamFrame();

    void handleGoawayFrame();

    void handleHeadersFrame();

    void handleDataFrame();

    void handlePriorityFrame();

    void handlePingFrame();

    void handleRequest(const HttpStream::Ptr &stream);

    void handleWrite();

    void writeSettingsFrame();

    void writeAckFrame();

    void writeGoawayFrame(
        uint32_t latest_stream_id,
        uint8_t flags,
        uint32_t error_code,
        const std::string &msg,
        bool once = false
    );

    void writeRstStreamFrame(
        uint32_t stream_id,
        uint8_t flags,
        uint32_t error_code,
        bool once = false
    );

    void writeWindowUpdateFrame(
        uint32_t stream_id,
        uint8_t flags,
        uint32_t window_size
    );

    /// Write HEADERS frame
    /// @param stream Operating stream
    /// @param verify_end_stream Whether to determine END_STREAM through response body
    /// @return Whether the current stream has been fully processed
    bool writeHeadersFrame(const HttpStream::Ptr &stream, bool verify_end_stream = true);

    /// Write the controller's response body into a DATA frame
    /// @param stream Operating stream
    /// @return Whether the current stream has been fully processed
    bool writeDataFrame4Body(const HttpStream::Ptr &stream);

    /// Write single-range file response into a DATA frame
    /// @param stream Operating stream
    /// @return Whether the current stream has been fully processed
    bool writeDataFrame4SingleRange(const HttpStream::Ptr &stream);

    /// Write multi-range file response into a DATA frame
    /// @param stream Operating stream
    /// @return Whether the current stream has been fully processed
    bool writeDataFrame4Ranges(const HttpStream::Ptr &stream);

    /// Process the first DATA frame of the range
    /// @warning This function is highly coupled and intended for reuse
    /// @param stream Operating stream
    /// @param subheader Range header
    /// @param remind Remaining window size after removing the range header
    /// @see writeDataFrame4Ranges
    void writeSubheaderAndData(const HttpStream::Ptr &stream, const std::string &subheader, size_t remind);
};

struct HttpConnectionExImpl final : HttpConnectionEx {
    using Ptr = std::shared_ptr<HttpConnectionExImpl>;
    using Socket = asio::ip::tcp::socket;
    using SharedSocket = std::shared_ptr<Socket>;

    Ptr getPtr() { return std::reinterpret_pointer_cast<HttpConnectionExImpl>(shared_from_this()); } // NOLINT

    SharedSocket socket;

    HttpConnectionExImpl(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &context,
                         const sese::net::IPAddress::Ptr &addr, SharedSocket socket);

    void writeBlocks(const std::vector<asio::const_buffer> &buffers,
                     const std::function<void(const asio::error_code &code)> &callback) override;

    void writeBlock(const void *buffer, size_t size,
                    const std::function<void(const asio::error_code &code)> &callback) override;

    void readBlock(char *buffer, size_t length,
                   const std::function<void(const asio::error_code &code)> &callback) override;

    void checkKeepalive() override;
};

struct HttpsConnectionExImpl final : HttpConnectionEx {
    using Ptr = std::shared_ptr<HttpsConnectionExImpl>;
    using Stream = asio::ssl::stream<asio::ip::tcp::socket>;
    using SharedStream = std::shared_ptr<Stream>;

    Ptr getPtr() { return std::reinterpret_pointer_cast<HttpsConnectionExImpl>(shared_from_this()); } // NOLINT

    SharedStream stream;

    HttpsConnectionExImpl(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &context,
                          const sese::net::IPAddress::Ptr &addr, SharedStream stream);

    void writeBlocks(const std::vector<asio::const_buffer> &buffers,
                     const std::function<void(const asio::error_code &code)> &callback) override;

    void writeBlock(const void *buffer, size_t size,
                    const std::function<void(const asio::error_code &code)> &callback) override;

    void readBlock(char *buffer, size_t length,
                   const std::function<void(const asio::error_code &code)> &callback) override;

    void checkKeepalive() override;
};

}