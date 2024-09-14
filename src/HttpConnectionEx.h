#pragma once

#include <asio.hpp>
#include <asio/ssl/stream.hpp>

#include <sese/net/http/Request.h>
#include <sese/net/http/Response.h>
#include <sese/net/http/DynamicTable.h>
#include <sese/net/http/Http2Frame.h>
#include <sese/io/ByteBuffer.h>

#include <memory>
#include <queue>
#include <set>

#include "Handleable.h"

class HttpServiceImpl;

struct HttpStream : Handleable {
    using Ptr = std::shared_ptr<HttpStream>;

    explicit HttpStream(uint32_t id, uint32_t write_window_size) noexcept;

    uint32_t id = 0;
    /// 对端写入窗口
    uint32_t write_window_size = 0;
    /// 本地读取窗口
    uint32_t read_window_size = 0;
    uint16_t continue_type = 0;
    bool end_headers = false;
    bool end_stream = false;

    sese::io::ByteBuilder temp_buffer;

    sese::net::http::Http2FrameInfo frame{};
};

struct HttpConnectionEx : std::enable_shared_from_this<HttpConnectionEx> {
    using Ptr = std::shared_ptr<HttpConnectionEx>;

    Ptr getPtr() { return shared_from_this(); } // NOLINT

    HttpConnectionEx(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &io_context);

    virtual ~HttpConnectionEx() = default;

    bool keepalive = false;
    asio::system_timer timer;

    std::weak_ptr<HttpServiceImpl> service;

    bool is_read = false;
    bool expect_ack = false;
    uint32_t accept_stream_count = 0;
    uint32_t latest_stream_ident = 0;

    // 本地最大帧大小
    static constexpr uint32_t MAX_FRAME_SIZE = 16384;
    // 本地初始窗口值
    static constexpr uint32_t INIT_WINDOW_SIZE = 65535;
    // 默认动态表大小
    static constexpr uint32_t HEADER_TABLE_SIZE = 8192;

    sese::net::http::Http2FrameInfo frame{};
    // 临时缓存，用于读取初始连接魔数、帧头，取最大帧大小
    char temp_buffer[MAX_FRAME_SIZE]{};
    uint32_t header_table_size = 4096;
    uint32_t enable_push = 0;
    uint32_t max_concurrent_stream = 0;
    // 对端初始窗口值
    uint32_t init_window_size = 65535;
    uint32_t max_frame_size = 16384;
    uint32_t max_header_list_size = 0;
    sese::net::http::DynamicTable req_dynamic_table;
    sese::net::http::DynamicTable resp_dynamic_table;
    std::map<uint32_t, HttpStream::Ptr> streams;
    std::set<uint32_t> closed_streams;

    /// 发送队列
    std::queue<sese::net::http::Http2Frame::Ptr> send_queue;

    /// 关闭流
    /// @param id 流 ID
    void close(uint32_t id);

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
        const std::string &msg
    );

    void writeRstStreamFrame(
        uint32_t stream_id,
        uint8_t flags,
        uint32_t error_code
    );

    void writeHeadersFrame(const HttpStream::Ptr &stream);
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

    void readBlock(char *buffer, size_t length,
                   const std::function<void(const asio::error_code &code)> &callback) override;
};

struct HttpsConnectionExImpl final : HttpConnectionEx {
    using Ptr = std::shared_ptr<HttpsConnectionExImpl>;
    using Stream = asio::ssl::stream<asio::ip::tcp::socket>;
    using SharedStream = std::shared_ptr<Stream>;

    Ptr getPtr() { return std::reinterpret_pointer_cast<HttpsConnectionExImpl>(shared_from_this()); } // NOLINT

    SharedStream stream;

    HttpsConnectionExImpl(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &context,
                          SharedStream stream);

    void writeBlock(const char *buffer, size_t length,
                    const std::function<void(const asio::error_code &code)> &callback) override;

    void readBlock(char *buffer, size_t length,
                   const std::function<void(const asio::error_code &code)> &callback) override;
};
