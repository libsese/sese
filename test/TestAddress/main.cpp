#include "net/IPv4Address.h"
#include "Util.h"

#define FILTER_TEST_ADDRESS "fAddress"

int main() {
    auto address = sese::Address::lookUpAny("www.baidu.com");
    ROOT_INFO(FILTER_TEST_ADDRESS, "www.baidu.com: %s", address->getAddress().c_str())
    return 0;
}