/**
 * @file Address.h
 * @author kaoru
 * @date 2022年4月4日
 * @brief 地址类
 */

#pragma once

#ifdef _WIN32
#ifdef __MINGW32__
#define _WIN32_WINNET 0x602
#endif
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#include "sese/Config.h"

#include <memory>
#include <string>
#include <vector>


namespace sese::net {

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
    static Ptr create(const sockaddr *address, socklen_t address_len);

    static bool lookUp(std::vector<Address::Ptr> &result, const std::string &host, int family = AF_INET, int type = 0, int protocol = 0);

    static Address::Ptr lookUpAny(const std::string &host, int family = AF_INET, int type = 0, int protocol = 0);

public:
    virtual ~Address() = default;
    [[nodiscard]] virtual sockaddr *getRawAddress() const noexcept = 0;
    [[nodiscard]] virtual socklen_t getRawAddressLength() const noexcept = 0;
    [[nodiscard]] virtual std::string getAddress() const noexcept = 0;
};

extern "C" int inetPton(int af, const char *src, void *dst) noexcept;
} // namespace sese::net