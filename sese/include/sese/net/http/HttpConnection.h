#pragma once

#include <sese/net/http/Request.h>
#include <sese/net/http/Response.h>
#include <sese/net/http/Range.h>
#include <sese/io/ByteBuilder.h>
#include <sese/io/FileStream.h>
#include <sese/service/TimerableService.h>

namespace sese::net::http {

    /// 指示 Http 处理状态
    enum class HttpHandleStatus {
        HANDING,/// 指示当前请求即将进入处理状态
        FAIL,   /// 指示当前请求处理失败，已无法返回任何报文
        OK,     /// 指示当前请求处理完成，需要返回报文
        FILE    /// 指示当前请求是一个下载文件的请求，需要在 onWrite 时读取文件
    };

    struct API HttpConnection final {
        int fd = 0;
        void *ssl = nullptr;

        HttpHandleStatus status = HttpHandleStatus::HANDING;
        net::http::Request req;
        net::http::Response resp;

        io::FileStream::Ptr file;
        size_t fileSize = 0;

        io::ByteBuilder buffer{4096};// 响应 Header 缓冲区
        uint64_t requestSize = 0; // 请求大小

        std::string contentType = "application/x-";           // 默认响应内容类型 - 仅在 status 为 FILE 且 存在多段 Range 时使用
        std::vector<net::http::Range> ranges;                 // 响应区间集合 - 仅在 status 为 FILE 时使用
        std::vector<net::http::Range>::iterator rangeIterator;// 指示当前响应区间迭代器

        event::BaseEvent *event = nullptr;
        service::TimeoutEvent *timeoutEvent = nullptr;

        ~HttpConnection() noexcept;
    };

}