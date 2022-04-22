#include "sese/net/IPv6Address.h"
#include "sese/record/LogHelper.h"

sese::LogHelper helper("fADDRESS"); // NOLINT

int main() {
    auto address = sese::Address::lookUpAny("www.baidu.com");
    if (address) {
        helper.info("www.baidu.com: %s", address->getAddress().c_str());
    }else{
        helper.info("Check your connect!");
    }

    auto address6 = sese::IPv6Address::create("fe80::ce6:3cc:f93a:4203", 0);
    helper.info("%s", address6->getAddress().c_str());
    return 0;
}