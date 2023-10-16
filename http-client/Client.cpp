#include <sese/util/Initializer.h>
#include <sese/record/Marco.h>
#include <sese/service/http/HttpClient_V1.h>
#include <sese/io/File.h>

int main() {
    sese::Initializer::getInitializer();

    sese::service::v1::HttpClient client;
    client.setThreads(1);
    if (client.init()) {
        SESE_INFO("Client initialization succeeded.");
    } else {
        SESE_ERROR("Client initialization failed.");
    }

    // auto file = sese::io::File::create("index.html", BINARY_WRITE_CREATE_TRUNC);

    auto handle = sese::service::v1::HttpClientHandle::create("http://cn.bing.com");
    handle->set("key", "value");
    handle->setConnectTimeout(30);
    // handle->setReadResponseBodyCallback([&file](void *buffer, size_t length) {
    //     file->write(buffer, length);
    // });

    client.post(handle);
    SESE_INFO("The request has been submitted.");
    auto status = handle->wait();
    if (status == sese::service::v1::HttpClientHandle::RequestStatus::Succeeded) {
        SESE_INFO("The request succeeded with %d.", handle->getStatusCode());
        handle->responseForEach([](const sese::service::v1::HttpClientHandle::HeaderForEachFunctionArgs &pair) {
            SESE_INFO("%s: %s", pair.first.c_str(), pair.second.c_str());
        });
        handle->cookieForEach([](const sese::net::http::Cookie::Ptr &cookie) {
            SESE_INFO("store cookie: %s", cookie->getName().c_str());
        });
    } else {
        SESE_WARN("The request failed.");
    }

    handle->setUrl("/index.html");
    client.post(handle);
    SESE_INFO("The request has been submitted.");
    status = handle->wait();
    if (status == sese::service::v1::HttpClientHandle::RequestStatus::Succeeded) {
        SESE_INFO("The request succeeded with %d.", handle->getStatusCode());
        handle->responseForEach([](const sese::service::v1::HttpClientHandle::HeaderForEachFunctionArgs &pair) {
            SESE_INFO("%s: %s", pair.first.c_str(), pair.second.c_str());
        });
        handle->cookieForEach([](const sese::net::http::Cookie::Ptr &cookie) {
            SESE_INFO("store cookie: %s", cookie->getName().c_str());
        });
    } else {
        SESE_WARN("The request failed.");
    }

    // file->close();
    client.shutdown();
    return 0;
}