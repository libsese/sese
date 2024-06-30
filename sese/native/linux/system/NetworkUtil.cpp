#include "sese/system/NetworkUtil.h"
#include "sese/net/IPv6Address.h"

#include <vector>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <net/if.h>
#include <unistd.h>
#include <map>

using namespace sese::system;

std::vector<NetworkInterface> NetworkUtil::getNetworkInterface() noexcept {
    std::vector<NetworkInterface> interfaces;
    std::map<std::string, NetworkInterface> map;
    struct ifaddrs *p_if_address = nullptr;

    // 这些信息仅用于获取网卡名称、 IPv4 和 mac 信息
    // glibc 2.3.3 以下不支持使用其获取 IPv6 相关信息
    getifaddrs(&p_if_address);

    auto p_address = p_if_address;
    while (p_address) {
        if (p_address->ifa_addr == nullptr) {
            p_address = p_address->ifa_next;
            continue;
        }

        if (p_address->ifa_addr->sa_family == AF_INET ||
            p_address->ifa_addr->sa_family == AF_PACKET) {

            auto iterator = map.find(p_address->ifa_name);
            if (iterator == map.end()) {
                auto i = NetworkInterface();
                iterator = map.insert({p_address->ifa_name, i}).first;
            }
            if (p_address->ifa_addr->sa_family == AF_INET) {
                sockaddr_in addr = *(sockaddr_in *) (p_address->ifa_addr);
                iterator->second.ipv4Addresses.emplace_back(std::make_shared<sese::net::IPv4Address>(addr));
            } else if (p_address->ifa_addr->sa_family == AF_PACKET) {
                iterator->second.name = p_address->ifa_name;
                memcpy(iterator->second.mac.data(), p_address->ifa_addr, 6);
            }
        }

        p_address = p_address->ifa_next;
    }

    freeifaddrs(p_if_address);

    // 用于获取 IPv6 信息
    FILE *f = fopen("/proc/net/if_inet6", "r");
    // 此处为假则表示不存在该配置文件，取消获取 IPv6 信息
    if (f != nullptr) { // GCOVR_EXCL_LINE
        int scope, prefix;
        unsigned char ipv6[16];
        char name[IFNAMSIZ];
        char address[INET6_ADDRSTRLEN];
        while (19 ==
               fscanf(f, // NOLINT
                      " %2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx %*x %x %x %*x %s",
                      &ipv6[0],
                      &ipv6[1],
                      &ipv6[2],
                      &ipv6[3],
                      &ipv6[4],
                      &ipv6[5],
                      &ipv6[6],
                      &ipv6[7],
                      &ipv6[8],
                      &ipv6[9],
                      &ipv6[10],
                      &ipv6[11],
                      &ipv6[12],
                      &ipv6[13],
                      &ipv6[14],
                      &ipv6[15],
                      &prefix,
                      &scope,
                      name)) {
            auto iterator = map.find(name);
            // 此处不可能为假，因为一定会存在 mac 地址信息
            // if (iterator != map.end()) {
            inet_ntop(AF_INET6, ipv6, address, sizeof(address));
            auto addr = sese::net::IPv6Address::create(address, 0);
            iterator->second.ipv6Addresses.emplace_back(addr);
            // }
        }

        fclose(f);
    }

    // 整合信息
    interfaces.reserve(map.size());
    for (decltype(auto) i: map) {
        // 防止出现空 name
        i.second.name = i.first;
        interfaces.emplace_back(i.second);
    }

    return interfaces;
}