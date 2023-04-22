#include <sese/net/AddressPool.h>

using namespace sese::net;

IPv4AddressPool IPv4AddressPool::pool;

IPv4Address::Ptr IPv4AddressPool::lookup(const std::string &domain) noexcept {
    auto iterator = pool.addressMap.find(domain);
    if (iterator == pool.addressMap.end()) {
        auto address = Address::lookUpAny(domain);
        if (nullptr == address) {
            // 未命中缓存且查找失败
            return nullptr;
        } else {
            auto ipv4 = dynamicPointerCast<IPv4Address>(address);
            pool.addressMap[domain] = ipv4;
            return ipv4;
        }
    } else {
        return iterator->second;
    }
}