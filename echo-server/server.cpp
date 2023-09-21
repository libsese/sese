#include <sese/service/iocp/IOCPServer.h>
#include <sese/record/Marco.h>
#include <sese/util/Initializer.h>

class MyIOCPServer : public sese::iocp::IOCPServer {
public:
    MyIOCPServer() {
        setDeleteContextCallback(myDeleter);
    }

    void onAcceptCompleted(Context *ctx) override {
        SESE_INFO("onAcceptCompleted %d", ctx->fd);
        postRead(ctx);
    }

    void onPreRead(Context *ctx) override {
        SESE_INFO("onRreRead %d", ctx->fd);
    }

    void onReadCompleted(Context *ctx) override {
        SESE_INFO("onReadCompleted %d", ctx->fd);
        ctx->recv.swap(ctx->send);
        postWrite(ctx);
    }

    void onWriteCompleted(Context *ctx) override {
        SESE_INFO("onWriteCompleted %d", ctx->fd);
        postRead(ctx);
    }

    static void myDeleter(Context *ctx) {
        SESE_INFO("onDeleteCallback %d", ctx->fd);
    }
};

MyIOCPServer server;

int main() {
    sese::Initializer::getInitializer();
    auto address = sese::net::IPv4Address::localhost(8080);
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
}