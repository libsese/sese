#include <sese/net/http/HttpServer.h>
#include <sese/record/LogHelper.h>
#include <sese/thread/Thread.h>
#include <atomic>

using sese::Initializer;
using sese::IPAddress;
using sese::Thread;
using sese::http::Cookie;
using sese::http::CookieMap;
using sese::http::HttpServer;
using sese::http::HttpServiceContext;
using sese::record::LogHelper;

static LogHelper logger("cookie");
static std::atomic_uint32_t id = 0;

void func(const HttpServiceContext::Ptr &context) {
    decltype(auto) req = context->getRequest();
    decltype(auto) resp = context->getResponse();

    CookieMap::Ptr cookies = req->getCookies();
    Cookie::Ptr cookie = nullptr;
    if (cookies != nullptr) {
        cookie = cookies->find("client_id");
    }

    std::string message;
    if (cookie != nullptr) {
        message = "client id = " + cookie->getValue();
        logger.debug("exist client id = %s", cookie->getValue().c_str());
    } else {
        id++;
        std::string idStr = std::to_string(id.load());
        logger.info("new client_id = %s", idStr.c_str());
        auto newCookies = std::make_shared<CookieMap>();
        auto c = std::make_shared<Cookie>("client_id", idStr);
        c->setExpires(1672490377);
        newCookies->add(c);
        resp->setCookies(newCookies);
        message = "client id = " + idStr;
    }

    resp->set("Content-Length", std::to_string(message.length()));
    context->flush();
    context->write(message.c_str(), message.length());
}

int main() {
    auto ip = IPAddress::create("0.0.0.0", 8080);
    auto server = HttpServer::create(ip);
    if (server == nullptr) {
        logger.debug("failed to init");
        exit(-1);
    }
    logger.debug("ok");

    auto th = Thread([&server]() {
        server->loopWith(func);
    },
                     "loop thread");
    th.start();

    getchar();
    logger.debug("want shutdown");
    server->shutdown();
    th.join();
    logger.debug("exited");
    return 0;
}