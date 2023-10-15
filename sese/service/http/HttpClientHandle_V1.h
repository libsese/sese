/// \file HttpClientHandle_V1.h
/// \brief Http 客户端句柄
/// \author kaoru
/// \date 2023年10月15日

#include <sese/service/iocp/IOCPServer.h>
#include <sese/net/IPv6Address.h>
#include <sese/net/http/Request.h>
#include <sese/net/http/Response.h>

#include <atomic>
#include <condition_variable>
#include <functional>

namespace sese::service::v1 {

class HttpClient;

class HttpClientHandle {
    friend class HttpClient;

public:
    using Ptr = std::shared_ptr<HttpClientHandle>;

    using Req = net::http::Request;
    using Resp = net::http::Response;
    using Cookie = net::http::Cookie;
    using Cookies = net::http::CookieMap;
    using HttpVersion = net::http::HttpVersion;
    using IPAddress = net::IPAddress;

    using WriteRequestBodyCallback = std::function<void(io::OutputStream *output, size_t *length)>;
    using ReadResponseBodyCallback = std::function<void(void *input, size_t length)>;
    using RequestDoneCallback = std::function<void(const HttpClientHandle::Ptr &handle)>;
    using RequestRetryCallback = std::function<void(const HttpClientHandle::Ptr &handle)>;

    enum class RequestStatus {
        /// 请求已经就绪，客户端可以进行连接
        Ready,
        /// 客户端正在等待连接完成
        Connecting,
        /// 客户端连接失败
        ConnectFailed,
        /// 客户端正在发送请求
        Requesting,
        /// 客户端请求失败
        RequestFailed,
        /// 客户端正在等待响应
        Responding,
        /// 客户端接收响应失败
        ResponseFailed,
        /// 请求已完成
        Succeeded
    };

    static HttpClientHandle::Ptr create(const std::string &url);

    // static HttpClientHandle::Ptr create(const IPAddress::Ptr address, bool ssl = false);

    [[nodiscard]] const net::http::Request::Ptr &getReq() const;
    [[nodiscard]] const net::http::Response::Ptr &getResp() const;

    [[nodiscard]] RequestStatus getStatus() const { return requestStatus; }
    RequestStatus wait();

    void setConnectTimeout(size_t timeout) { HttpClientHandle::connectTimeout = timeout; }
    void setRequestTimeout(size_t timeout) { HttpClientHandle::requestTimeout = timeout; }
    void setResponseTimeout(size_t timeout) { HttpClientHandle::responseTimeout = timeout; }

private:
    HttpClientHandle() = default;

    bool ssl = false;
    security::SSLContext::Ptr clientCtx{};

    size_t retryTimes = 3;
    size_t triedTimes = 0;

    size_t connectTimeout = 30;
    size_t requestTimeout = 30;
    size_t responseTimeout = 30;

    size_t requestBodySize = 0;
    size_t requestBodyHandled = 0;

    size_t responseBodySize = 0;
    size_t responseBodyHandled = 0;

    std::atomic<RequestStatus> requestStatus{RequestStatus::Ready};

    Req::Ptr req;
    Resp::Ptr resp;
    Cookies::Ptr cookies;
    HttpVersion version{HttpVersion::VERSION_1_1};
    IPAddress::Ptr address;

    std::mutex mutex;
    std::condition_variable conditionVariable;
    WriteRequestBodyCallback writeRequestBodyCallback;
    ReadResponseBodyCallback readResponseBodyCallback;
    RequestDoneCallback requestDoneCallback;
    RequestRetryCallback requestRetryCallback;
};

} // namespace sese::service::v1
