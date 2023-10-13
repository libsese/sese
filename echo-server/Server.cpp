#include <sese/service/iocp/IOCPServer.h>
#include <sese/record/Marco.h>
#include <sese/util/Initializer.h>
#include <sese/util/Util.h>

class MyIOCPServer : public sese::iocp::IOCPServer {
public:
    MyIOCPServer() {
        setDeleteContextCallback(myDeleter);
    }

    void onAcceptCompleted(sese::iocp::Context *ctx) override {
        SESE_INFO("onAcceptCompleted %d", ctx->getFd());
        postRead(ctx);
        setTimeout(ctx, 10);
    }

    void onPreRead(sese::iocp::Context *ctx) override {
        cancelTimeout(ctx);
        SESE_INFO("onPreRead %d", ctx->getFd());
    }

    void onReadCompleted(sese::iocp::Context *ctx) override {
        SESE_INFO("onReadCompleted %d", ctx->getFd());
        sese::streamMove(ctx, ctx, IOCP_WSABUF_SIZE);
        postWrite(ctx);
    }

    void onWriteCompleted(sese::iocp::Context *ctx) override {
        SESE_INFO("onWriteCompleted %d", ctx->getFd());
        postRead(ctx);
        setTimeout(ctx, 10);
    }

    void onTimeout(sese::iocp::Context *ctx) override {
        SESE_INFO("onTimeout %d", ctx->getFd());
        postClose(ctx);
    }

    static void myDeleter(sese::iocp::Context *ctx) {
        SESE_INFO("onDeleteCallback %d", ctx->getFd());
    }
};

MyIOCPServer server;

int main() {
    sese::Initializer::getInitializer();
    auto address = sese::net::IPv4Address::any(8080);
    server.setThreads(2);
    server.setAddress(address);
    auto rt = server.init();
    if (!rt) {
        SESE_ERROR("server init failed!");
        return 0;
    }
    SESE_INFO("server listening on %d", address->getPort());
    getchar();
    server.shutdown();
    return 0;
}