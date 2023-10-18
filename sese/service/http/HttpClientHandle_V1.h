/// \file HttpClientHandle_V1.h
/// \brief Http 客户端句柄
/// \author kaoru
/// \date 2023年10月15日

#include <sese/service/iocp/IOCPServer.h>
#include <sese/net/IPv6Address.h>
#include <sese/net/http/Request.h>
#include <sese/net/http/Response.h>

#include <future>
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

    using HeaderForEachFunctionArgs = std::pair<std::string, const std::string>;
    using HeaderForEachFunction = std::function<void(const HeaderForEachFunctionArgs &)>;
    using CookieForEachFunction = std::function<void(const net::http::Cookie::Ptr &)>;

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
    static HttpClientHandle::Ptr create(const IPAddress::Ptr &address, const security::SSLContext::Ptr &ctx);

    [[nodiscard]] const net::http::Request::Ptr &getReq() const { return req; };
    [[nodiscard]] const net::http::Response::Ptr &getResp() const { return resp; };
    [[nodiscard]] const net::http::CookieMap::Ptr &getCookies() const { return cookies; };

    [[nodiscard]] const std::string &get(const std::string &name, const std::string &def) const { return resp->get(name, def); }
    void set(const std::string &name, const std::string &value) { req->set(name, value); }
    void setUrl(const std::string &url) { req->setUrl(url); }
    void setType(net::http::RequestType type) { req->setType(type); }
    void setContentLength(size_t length) { requestBodySize = length; }
    [[nodiscard]] uint16_t getStatusCode() const { return resp->getCode(); }
    int64_t writeBody(const void *buffer, size_t length) { return req->getBody().write(buffer, length); }
    int64_t readBody(void *buffer, size_t length) { return resp->getBody().read(buffer, length); }

    [[nodiscard]] RequestStatus getStatus() const { return requestStatus; }

    void setConnectTimeout(size_t timeout) { HttpClientHandle::connectTimeout = timeout; }
    void setRequestTimeout(size_t timeout) { HttpClientHandle::requestTimeout = timeout; }
    void setResponseTimeout(size_t timeout) { HttpClientHandle::responseTimeout = timeout; }

    void setWriteRequestBodyCallback(const WriteRequestBodyCallback &callback) { HttpClientHandle::writeRequestBodyCallback = callback; }
    void setReadResponseBodyCallback(const ReadResponseBodyCallback &callback) { HttpClientHandle::readResponseBodyCallback = callback; }
    void setRequestDoneCallback(const RequestDoneCallback &callback) { HttpClientHandle::requestDoneCallback = callback; }

    void requestForEach(const HeaderForEachFunction &func) const;
    void responseForEach(const HeaderForEachFunction &func) const;
    void cookieForEach(const CookieForEachFunction &func) const;

private:
    HttpClientHandle() = default;

    bool ssl = false;
    security::SSLContext::Ptr clientCtx{};

    size_t connectTimeout = 30;
    size_t requestTimeout = 30;
    size_t responseTimeout = 30;

    size_t requestBodySize = 0;
    size_t requestBodyHandled = 0;

    size_t responseBodySize = 0;
    size_t responseBodyHandled = 0;

    uint8_t triedTimes = 0;

    std::promise<RequestStatus> promise;
    RequestStatus requestStatus{RequestStatus::Ready};
    iocp::v1::Context *context{};

    Req::Ptr req;
    Resp::Ptr resp;
    Cookies::Ptr cookies;
    HttpVersion version{HttpVersion::VERSION_1_1};
    IPAddress::Ptr address;

    WriteRequestBodyCallback writeRequestBodyCallback;
    ReadResponseBodyCallback readResponseBodyCallback;
    RequestDoneCallback requestDoneCallback;
};

} // namespace sese::service::v1
