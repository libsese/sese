#include <sese/net/http/QueryString.h>
#include <sese/record/LogHelper.h>

using sese::LogHelper;
using sese::http::QueryString;

LogHelper helper("fHTTP"); //NOLINT

void testQueryString() {
    std::string rawUrl = "/index.html?val0=888&val1=&val2=1";
    helper.info("rawUrl: %s", rawUrl.c_str());
    auto queryString = QueryString();
    queryString.parse(rawUrl);

    helper.info("url: %s", queryString.getUrl().c_str());
    for (const auto& item: queryString) {
        helper.info("%s: %s", item.first.c_str(), item.second.c_str());
    }

    queryString.set("val1", "999");
    helper.info("toString: %s", queryString.toString().c_str());
}

int main() {
    testQueryString();
    return 0;
}