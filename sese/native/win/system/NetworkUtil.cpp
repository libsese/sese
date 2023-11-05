#include "sese/system/NetworkUtil.h"
#include "sese/net/IPv6Address.h"
#include <vector>

#include <iphlpapi.h>

using namespace sese::system;

std::vector<NetworkInterface> NetworkUtil::getNetworkInterface() noexcept {
    ULONG rt = 0;
    ULONG size = 0;
    std::unique_ptr<CHAR[]> buffer;
    PIP_ADAPTER_ADDRESSES pAddresses = nullptr;

    std::vector<NetworkInterface> result;

    rt = GetAdaptersAddresses(
            AF_UNSPEC,
            GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER,
            nullptr,
            pAddresses,
            &size
    );

    // 此处必为 ERROR_BUFFER_OVERFLOW，作用仅为获取实际所需大小
    if (rt != ERROR_BUFFER_OVERFLOW) {
        return result;
    }

    buffer = std::make_unique<CHAR[]>(size);
    pAddresses = (PIP_ADAPTER_ADDRESSES) buffer.get();

    rt = GetAdaptersAddresses(
            AF_UNSPEC,
            GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER,
            nullptr,
            pAddresses,
            &size
    );

    // 此处为 ERROR_SUCCESS 为成功
    if (rt != ERROR_SUCCESS) {
        return result;
    }

    NetworkInterface Interface;
    decltype(pAddresses->FirstUnicastAddress) unicastAddress;
    while (pAddresses) {
        if (pAddresses->IfType == IF_TYPE_ETHERNET_CSMACD) {
            Interface.name = pAddresses->AdapterName;

            unicastAddress = pAddresses->FirstUnicastAddress;
            while (unicastAddress) {
                auto pSockAddr = (SOCKADDR *) unicastAddress->Address.lpSockaddr;
                if (pSockAddr->sa_family == AF_INET) {
                    sockaddr_in sockaddrIn = *(sockaddr_in *) unicastAddress->Address.lpSockaddr;
                    Interface.ipv4Addresses.emplace_back(std::make_shared<sese::net::IPv4Address>(sockaddrIn));
                } else if (pSockAddr->sa_family == AF_INET6) {
                    sockaddr_in6 sockaddrIn6 = *(sockaddr_in6 *) unicastAddress->Address.lpSockaddr;
                    Interface.ipv6Addresses.emplace_back(std::make_shared<sese::net::IPv6Address>(sockaddrIn6));
                }
                unicastAddress = unicastAddress->Next;
            }

            memcpy(Interface.mac.data(), pAddresses->PhysicalAddress, 6);

            result.emplace_back(Interface);
            Interface.ipv4Addresses.clear();
            Interface.ipv6Addresses.clear();
        }

        pAddresses = pAddresses->Next;
    }

    return result;
}