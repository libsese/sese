#pragma once

#include "sese/net/V2Server.h"
#include "sese/net/http/Http2Connection.h"
#include "sese/net/http/Http2FrameInfo.h"
#include "sese/net/http/Huffman.h"

#include <map>

namespace sese::net::v2::http {

    class API [[deprecated("此实现是不完整的")]] Http2Server : public Server {
    public:
        void onConnect(IOContext &ctx) noexcept override;

        void onHandle(IOContext &ctx) noexcept override;

        void onClosing(IOContext &tx) noexcept override;

        void onHttpHandle(IOContext &ctx) noexcept;

        void onHttp2Handle(IOContext &ctx, net::http::Http2Connection::Ptr conn) noexcept;

    public:
        static bool readFrame(IOContext &ctx, sese::net::http::Http2FrameInfo &info) noexcept;

        static void sendGoaway(IOContext &ctx, uint32_t sid, uint32_t eid) noexcept;

        bool decode(InputStream *input, net::http::DynamicTable &dynamicTable, net::http::Header &header) noexcept;

    protected:
        static void decodeInteger(uint8_t &buf, InputStream *input, uint32_t &dest, uint8_t n) noexcept;

        std::optional<std::string> decodeString(InputStream *input) noexcept;

    protected:
        // 对 connMap 操作加锁
        std::mutex mutex;
        std::map<socket_t, net::http::Http2Connection::Ptr> connMap;

        net::http::HuffmanDecoder decoder;
        net::http::HuffmanEncoder encoder;
    };

}// namespace sese::net::v2::http