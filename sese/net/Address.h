/**
 * @file Address.h
 * @author kaoru
 * @date 2022年4月4日
 * @brief 地址类
 */
#pragma once
#ifdef _WIN32
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif
#include "sese/Config.h"
#include <memory>
#include <string>
#include <vector>


namespace sese {

    template<typename T>
    static T CreateMask(uint32_t bits) {
        return (1 << (sizeof(T) * 8 - bits)) - 1;
    }

    /**
     * @brief 地址类
     */
    class API Address {
    public:
        using Ptr = std::shared_ptr<Address>;

    public:
        static Ptr create(const sockaddr *address, socklen_t addressLen);

        static bool lookUp(std::vector<Address::Ptr> &result, const std::string &host, int family = AF_INET, int type = 0, int protocol = 0);

        static Address::Ptr lookUpAny(const std::string &host, int family = AF_INET, int type = 0, int protocol = 0);

    public:
        virtual ~Address() = default;
        [[nodiscard]] virtual sockaddr *getRawAddress() const noexcept = 0;
        [[nodiscard]] virtual socklen_t getRawAddressLength() const noexcept = 0;
        [[nodiscard]] virtual std::string getAddress() const noexcept = 0;
    };
}// namespace sese