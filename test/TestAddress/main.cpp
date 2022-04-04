#include "net/IPv6Address.h"
#include "Util.h"

#define FILTER_TEST_ADDRESS "fAddress"

int main() {
    auto address = sese::Address::lookUpAny("www.baidu.com");
    if (address) {
        ROOT_INFO(FILTER_TEST_ADDRESS, "www.baidu.com: %s", address->getAddress().c_str())
    }else{
        ROOT_INFO(FILTER_TEST_ADDRESS, "Check your connect!")
    }

    auto address6 = sese::IPv6Address::create("fe80::ce6:3cc:f93a:4203", 0);
    ROOT_INFO(FILTER_TEST_ADDRESS, "%s", address6->getAddress().c_str())
    return 0;
}