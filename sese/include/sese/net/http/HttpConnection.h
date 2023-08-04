#pragma once

#include <sese/net/http/RequestHeader.h>
#include <sese/net/http/ResponseHeader.h>
#include <sese/net/http/Range.h>
#include <sese/util/ByteBuilder.h>
#include <sese/util/FileStream.h>
#include <sese/service/TimerableService.h>

namespace sese::net::http {

    /// 指示 Http 处理状态
    enum class HttpHandleStatus {
        HANDING,/// 指示当前请求即将进入处理状态
        FAIL,   /// 指示当前请求处理失败，已无法返回任何报文
        OK,     /// 指示当前请求处理完成，需要返回报文
        FILE    /// 指示当前请求是一个下载文件的请求，需要在 onWrite 时读取文件
    };

    struct API HttpConnection final : public sese::Stream {
        int fd = 0;
        void *ssl = nullptr;

        HttpHandleStatus status = HttpHandleStatus::HANDING;
        net::http::RequestHeader req;
        net::http::ResponseHeader resp;

        FileStream::Ptr file;
        size_t fileSize = 0;

        ByteBuilder buffer1{4096};// 请求缓冲区
        ByteBuilder buffer2{4096};// 响应缓冲区
        uint64_t requestSize = 0; // 请求大小

        std::string contentType = "application/x-";           // 默认响应内容类型 - 仅在 status 为 FILE 且 存在多段 Range 时使用
        std::vector<net::http::Range> ranges;                 // 响应区间集合 - 仅在 status 为 FILE 时使用
        std::vector<net::http::Range>::iterator rangeIterator;// 指示当前响应区间迭代器

        event::BaseEvent *event = nullptr;
        service::TimeoutEvent *timeoutEvent = nullptr;

        ~HttpConnection() noexcept;

        void doResponse() noexcept;

        int64_t read(void *buf, size_t len) override;

        int64_t write(const void *buf, size_t len) override;
    };

}