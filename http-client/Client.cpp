#include <sese/util/Initializer.h>
#include <sese/record/Marco.h>
#include <sese/service/http/HttpClient_V1.h>

int main() {
    sese::Initializer::getInitializer();

    sese::service::v1::HttpClient client;
    client.setThreads(2);
    if (client.init()) {
        SESE_INFO("Client initialization succeeded.");
    } else {
        SESE_ERROR("Client initialization failed.");
    }

    auto handle = sese::service::v1::HttpClientHandle::create("https://bing.com");
    handle->getReq()->set("key", "value");

    client.post(handle);
    SESE_INFO("The request has been submitted.");
    auto status = handle->wait();
    if (status == sese::service::v1::HttpClientHandle::RequestStatus::Succeeded) {
        SESE_INFO("The request succeeded.");
    } else {
        SESE_WARN("The request failed.");
    }

    client.shutdown();
    return 0;
}