#include <sese/service/iocp/IOCPServer.h>
#include <sese/record/Marco.h>
#include <sese/util/Initializer.h>
#include <sese/util/Util.h>

class MyIOCPServer : public sese::iocp::IOCPServer {
public:
    MyIOCPServer() {
        setDeleteContextCallback(myDeleter);
    }

    void onAcceptCompleted(Context *ctx) override {
        SESE_INFO("onAcceptCompleted %d", (int) ctx->fd);
        postRead(ctx);
    }

    void onPreRead(Context *ctx) override {
        SESE_INFO("onRreRead %d", (int) ctx->fd);
    }

    void onReadCompleted(Context *ctx) override {
        SESE_INFO("onReadCompleted %d", (int) ctx->fd);
        // ctx->recv.swap(ctx->send);
        sese::streamMove(&ctx->send, &ctx->recv, IOCP_WSABUF_SIZE);
        postWrite(ctx);
    }

    void onWriteCompleted(Context *ctx) override {
        SESE_INFO("onWriteCompleted %d", (int) ctx->fd);
        postRead(ctx);
    }

    static void myDeleter(Context *ctx) {
        SESE_INFO("onDeleteCallback %d", (int) ctx->fd);
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
}