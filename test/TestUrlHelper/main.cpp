#include <sese/net/http/UrlHelper.h>
#include <sese/record/LogHelper.h>
#include <sese/Test.h>

using namespace sese::http;
using sese::LogHelper;

LogHelper helper("fURL");// NOLINT

int main() {
    std::string url = "https://www.baidu.com/index.html?query=github";

    UrlInfo info;
    UrlHelper::parse(url, info);

    assert(helper, info.protocol == "https", 0);
    assert(helper, info.host == "www.baidu.com", 0);
    assert(helper, info.uri == "/index.html", 0);
    assert(helper, info.query == "?query=github", 0);
    return 0;
}