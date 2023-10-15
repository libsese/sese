/// \file HttpClient_V1.h
/// \brief Http 客户端
/// \author kaoru
/// \date 2023年10月15日

#include <sese/service/http/HttpClientHandle_V1.h>

namespace sese::service::v1 {

class HttpClient final : public iocp::v1::IOCPServer {
public:
    using Supper = iocp::v1::IOCPServer;
    using Context = iocp::v1::Context;

    HttpClient();

    void post(const HttpClientHandle::Ptr &handle);

private:
    struct Data {
        HttpClientHandle::Ptr handle{};
    };

    static void deleter(sese::iocp::Context *ctx);

    void onPreRead(Context *ctx) override;
    void onReadCompleted(Context *ctx) override;
    void onWriteCompleted(Context *ctx) override;
    void onTimeout(Context *ctx) override;
    void onPreConnect(Context *ctx) override;
    void onConnected(Context *ctx) override;

    using Supper::cancelTimeout;
    using Supper::getDeleteContextCallback;
    using Supper::getServCtx;
    using Supper::onAcceptCompleted;
    using Supper::onAlpnSelect;
    using Supper::onDeleteContext;
    using Supper::postClose;
    using Supper::postConnect;
    using Supper::postRead;
    using Supper::postWrite;
    using Supper::setAddress;
    using Supper::setDeleteContextCallback;
    using Supper::setServCtx;
    using Supper::setServProtos;
    using Supper::setTimeout;
};

} // namespace sese::service::v1