/**
 * @file IPv6Address.h
 * @author kaoru
 * @brief IPv6 地址类
 * @date 2022年4月4日
 */
#pragma once
#include "IPv4Address.h"

namespace sese {

    /**
     * @brief IPv6 地址类
     */
    class API IPv6Address : public IPAddress {
    public:
        using Ptr = std::shared_ptr<IPv6Address>;
        static Ptr create(const char *address, uint16_t port);

    public:
        explicit IPv6Address() noexcept;
        explicit IPv6Address(const sockaddr_in6 &address) noexcept;
        explicit IPv6Address(uint8_t *address, uint16_t port = 0);

        [[nodiscard]] sockaddr *getRawAddress() const noexcept override;
        [[nodiscard]] socklen_t getRawAddressLength() const noexcept override;
        [[nodiscard]] std::string getAddress() const noexcept override;

        [[nodiscard]] IPAddress::Ptr getBroadcastAddress(uint32_t prefixLen) const noexcept override;
        [[nodiscard]] IPAddress::Ptr getNetworkAddress(uint32_t prefixLen) const noexcept override;
        [[nodiscard]] IPAddress::Ptr getSubnetMask(uint32_t prefixLen) const noexcept override;

        void setPort(uint16_t port) noexcept override { this->address.sin6_port = ToBigEndian16(port); }
        [[nodiscard]] uint16_t getPort() const noexcept override {
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) || defined(_WIN32)
            return ByteSwap16(address.sin6_port);
#else
            return address.sin6_port;
#endif
        }

    private:
        sockaddr_in6 address{0};
    };
}// namespace sese