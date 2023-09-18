//#include <sese/net/http/HttpServer.h>
//#include <sese/net/http/HttpUtil.h>
//#include <sese/security/SSLContextBuilder.h>
//#include <sese/util/Util.h>
//
//using sese::Stream;
//using sese::http::HttpServiceContext;
//using sese::http::HttpServer;
//using sese::http::HttpsServer;
//using sese::http::HttpRequest;
//using sese::http::HttpResponse;
//using sese::http::HttpUtil;
//
//HttpServer::Ptr HttpServer::create(const IPAddress::Ptr &ipAddress, size_t threads, size_t keepAlive) noexcept {
//    auto tcpServer = TcpServer::create(ipAddress, threads, keepAlive);
//    if (nullptr == tcpServer) {
//        return nullptr;
//    } else {
//        auto server = new HttpServer;
//        server->tcpServer = std::move(tcpServer);
//        return std::unique_ptr<HttpServer>(server);
//    }
//}
//
//void HttpServer::loopWith(const HttpServer::Handler &handler) {
//    tcpServer->loopWith([&](IOContext *ioContext) {
//        ServiceContext context;
//        context.reset(ioContext);
//
//        auto request = &context.request;
//
//        if (!HttpUtil::recvRequest(&context, &context.request)) {
//            context.close();
//            return;
//        }
//
//        bool isKeepAlive = tcpServer->getKeepAlive() != 0 && 0 == strcasecmp(request->get("Connection", "Keep-Alive").c_str(), "Keep-Alive");
//
//        handler(&context);
//
//        if (!context.isFlushed()) {
//            if (!context.flush()) {
//                context.close();
//                return;
//            }
//        }
//
//        if (!isKeepAlive) {
//            context.close();
//            return;
//        }
//    });
//}
//
//void HttpServer::shutdown() {
//    tcpServer->shutdown();
//}
//
//HttpsServer::Ptr HttpsServer::create(const IPAddress::Ptr &ipAddress, size_t threads, size_t keepAlive, SecurityOptions options) noexcept {
//    auto ctx = security::SSLContextBuilder::SSL4Server();
//    ctx->importCertFile(options.CA);
//    ctx->importPrivateKey(options.KEY);
//
//    auto tcpServer = security::SecurityTcpServer::create(ipAddress, threads, keepAlive, ctx);
//    if (nullptr == tcpServer) {
//        return nullptr;
//    } else {
//        auto server = new HttpsServer;
//        server->tcpServer = std::move(tcpServer);
//        return std::unique_ptr<HttpsServer>(server);
//    }
//}
//
//void HttpsServer::loopWith(const Handler &handler) {
//    tcpServer->loopWith([&](security::IOContext *ioContext) {
//        ServiceContext context;
//        context.reset(ioContext);
//
//        auto request = &context.request;
//
//        if (!HttpUtil::recvRequest(&context, &context.request)) {
//            context.close();
//            return;
//        }
//
//        bool isKeepAlive = tcpServer->getKeepAlive() != 0 && 0 == strcasecmp(request->get("Connection", "Keep-Alive").c_str(), "Keep-Alive");
//
//        handler(&context);
//
//        if (!context.isFlushed()) {
//            if (!context.flush()) {
//                context.close();
//                return;
//            }
//        }
//
//        if (!isKeepAlive) {
//            context.close();
//            return;
//        }
//    });
//}
//
//void HttpsServer::shutdown() {
//    tcpServer->shutdown();
//}