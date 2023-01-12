#include <sese/net/http/HttpClient.h>
#include <sese/record/LogHelper.h>
#include "sese/util/Test.h"

using sese::record::LogHelper;
using sese::http::HttpClient;
using sese::http::RequestHeader;
using sese::http::ResponseHeader;

int main() {
    sese::record::LogHelper helper("CLIENT");

    RequestHeader header;
    header.setUrl("/");
    header.set("Content-Length", "0")
            ->set("Content-Type", "None")
            ->set("Message", "A message from my client");

    auto client = HttpClient::create("www.baidu.com");
    assert(helper, client != nullptr, -1);
    assert(helper, client->request(&header), -2);

    auto response = client->response();
    assert(helper, response != nullptr, -3);

    helper.info("response code: %d", response->getCode());
    for(decltype(auto) pair : *response) {
        printf("%s: %s\n", pair.first.c_str(), pair.second.c_str());
    }

    auto len = client->getResponseContentLength();
    printf("\nand %d bytes of content ...\n", (int) len);

    return 0;
}