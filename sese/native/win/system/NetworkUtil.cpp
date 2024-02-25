#include "sese/system/NetworkUtil.h"
#include "sese/net/IPv6Address.h"
#include <vector>

#include <iphlpapi.h>

using namespace sese::system;

std::vector<NetworkInterface> NetworkUtil::getNetworkInterface() noexcept {
    ULONG size = 0;
    std::unique_ptr<CHAR[]> buffer;
    PIP_ADAPTER_ADDRESSES p_addresses = nullptr;

    std::vector<NetworkInterface> result;

    ULONG rt = GetAdaptersAddresses(
        AF_UNSPEC,
        GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER,
        nullptr,
        p_addresses,
        &size
    );

    // 此处必为 ERROR_BUFFER_OVERFLOW，作用仅为获取实际所需大小
    if (rt != ERROR_BUFFER_OVERFLOW) {
        return result;
    }

    buffer = std::make_unique<CHAR[]>(size);
    p_addresses = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.get());

    rt = GetAdaptersAddresses(
            AF_UNSPEC,
            GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER,
            nullptr,
            p_addresses,
            &size
    );

    // 此处为 ERROR_SUCCESS 为成功
    if (rt != ERROR_SUCCESS) {
        return result;
    }

    NetworkInterface inter;
    while (p_addresses) {
        if (p_addresses->IfType == IF_TYPE_ETHERNET_CSMACD) {
            inter.name = p_addresses->AdapterName;

            decltype(p_addresses->FirstUnicastAddress) unicast_address = p_addresses->FirstUnicastAddress;
            while (unicast_address) {
                auto p_sock_addr = unicast_address->Address.lpSockaddr;
                if (p_sock_addr->sa_family == AF_INET) {
                    sockaddr_in sockaddr = *reinterpret_cast<sockaddr_in *>(unicast_address->Address.lpSockaddr);
                    inter.ipv4Addresses.emplace_back(std::make_shared<sese::net::IPv4Address>(sockaddr));
                } else if (p_sock_addr->sa_family == AF_INET6) {
                    sockaddr_in6 sockaddr6 = *reinterpret_cast<sockaddr_in6 *>(unicast_address->Address.lpSockaddr);
                    inter.ipv6Addresses.emplace_back(std::make_shared<sese::net::IPv6Address>(sockaddr6));
                }
                unicast_address = unicast_address->Next;
            }

            memcpy(inter.mac.data(), p_addresses->PhysicalAddress, 6);

            result.emplace_back(inter);
            inter.ipv4Addresses.clear();
            inter.ipv6Addresses.clear();
        }

        p_addresses = p_addresses->Next;
    }

    return result;
}