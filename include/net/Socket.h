/**
 * @file Socket.h
 * @author kaoru
 * @brief Native Socket 类
 * @date 2022年4月9日
 */
#pragma once
#include "IPv6Address.h"
#include "Config.h"
#include "Stream.h"

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese {

    /**
     * @brief Native Socket 类
     */
    class API Socket : public Stream {
    public:
        using Ptr = std::shared_ptr<Socket>;

        enum class Type {
            TCP = SOCK_STREAM,
            UDP = SOCK_DGRAM
        };

        enum class Family {
            IPv4 = AF_INET,
            IPv6 = AF_INET6
        };

        enum class ShutdownMode{
#ifdef _WIN32
            Read = 0,
            Write = 1,
            Both = 2
#else
            Read = SHUT_RD,
            Write = SHUT_WR,
            Both = SHUT_RDWR
#endif
        };

    public:
        Socket(socket_t handle, Address::Ptr address) noexcept;
        Socket(Family family, Type type, int32_t protocol = IPPROTO_IP) noexcept;
        ~Socket();

    public:
        int32_t bind(Address::Ptr addr) noexcept;
        int32_t connect(Address::Ptr addr) noexcept;
        [[nodiscard]] int32_t listen(int32_t backlog) const noexcept;
        [[nodiscard]] Socket::Ptr accept() const;
        [[nodiscard]] int32_t shutdown(ShutdownMode mode) const;

        /**
         * TCP 接收字节
         * @param buffer 缓存
         * @param length 缓存大小
         * @return 实际接收字节数
         */
        int64_t read(void *buffer, size_t length) override;
        /**
         * TCP 发送字节
         * @param buffer 缓存
         * @param length 缓存大小
         * @return 实际发送字节数
         */
        int64_t write(void *buffer, size_t length) override;

        /**
         * UDP 发送字节
         * @param buffer 缓存
         * @param length 缓存大小
         * @param to 目标地址
         * @param flags 标志
         * @return 实际发送字节数
         */
        int64_t send(void *buffer, size_t length, const IPAddress::Ptr &to, int32_t flags) const;
        /**
         * UDP 接收字节
         * @param buffer 缓存
         * @param length 缓存大小
         * @param from 来源地址
         * @param flags 标志
         * @return 实际接收字节数
         */
        int64_t recv(void *buffer, size_t length, const IPAddress::Ptr &from, int32_t flags) const;

        void close() override;

        [[nodiscard]] const socket_t &getRawSocket() const { return handle; }
        [[nodiscard]] const Address::Ptr &getAddress() const { return address; }

    private:
        socket_t handle{};
        Address::Ptr address;
    };

}// namespace sese