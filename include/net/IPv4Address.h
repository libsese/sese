#pragma once
#include "IPAddress.h"

namespace sese {

    class API IPv4Address : public IPAddress {
    public:
        using Ptr = std::shared_ptr<IPv4Address>;
        static Ptr create(const char *address, uint16_t port);

    public:
        explicit IPv4Address(const sockaddr_in &address);
        explicit IPv4Address(uint32_t address = INADDR_ANY, uint16_t port = 0);

    public:
        [[nodiscard]] const sockaddr *getRawAddress() const noexcept override;
        [[nodiscard]] socklen_t getRawAddressLength() const noexcept override;
        [[nodiscard]] std::string getAddress() const noexcept override;

        [[nodiscard]] IPAddress::Ptr getBroadcastAddress(uint32_t prefixLen) const noexcept override;
        [[nodiscard]] IPAddress::Ptr getNetworkAddress(uint32_t prefixLen) const noexcept override;
        [[nodiscard]] IPAddress::Ptr getSubnetMask(uint32_t prefixLen) const noexcept override;

    private:
        sockaddr_in address{0};
    };

}// namespace sese