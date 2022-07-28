#include <sese/net/AddressPool.h>
#include <sese/record/LogHelper.h>
#include <sese/Test.h>

using sese::IPv4AddressPool;
using sese::LogHelper;

LogHelper helper("AddressPool");// NOLINT

void ipv4() {
    auto address0 = IPv4AddressPool::lookup("www.baidu.com");
    auto address1 = IPv4AddressPool::lookup("www.baidu.com");
    assert(helper, address0.get() == address1.get(), 0)
}

int main() {
    ipv4();

    return 0;
}