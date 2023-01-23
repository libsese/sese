/**
 * @file Socket.h
 * @author kaoru
 * @brief Native Socket 类
 * @date 2022年4月9日
 */
#pragma once
#include "sese/net/IPv6Address.h"
#include "sese/Config.h"
#include "sese/util/Stream.h"
#include "sese/util/Initializer.h"

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese {

#ifdef _WIN32
    class SocketInitiateTask : public InitiateTask {
    public:
        SocketInitiateTask() : InitiateTask(__FUNCTION__) {}

        int32_t init() noexcept override;
        int32_t destroy() noexcept override;
    };
#endif

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

        enum class ShutdownMode {
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
        Socket(Family family, Type type, int32_t protocol = IPPROTO_IP) noexcept;
        Socket(socket_t handle, Address::Ptr address) noexcept;
        ~Socket() noexcept;
    public:
        virtual int32_t bind(Address::Ptr address) noexcept;
        virtual int32_t connect(Address::Ptr address) noexcept;
        virtual int32_t listen(int32_t backlog) const noexcept; /* NOLINT */
        [[nodiscard]] virtual Socket::Ptr accept() const;
        virtual int32_t shutdown(ShutdownMode mode) const; /* NOLINT */
        bool setNonblocking(bool enable) const noexcept; /* NOLINT */
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
        int64_t write(const void *buffer, size_t length) override;
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
    public:
#define W(func)          \
    value = func(value); \
    return write(&value, sizeof(value));
        int64_t writeInt16(int16_t value) { W(ToBigEndian16) }
        int64_t writeInt32(int32_t value) { W(ToBigEndian32) }
        int64_t writeInt64(int64_t value) { W(ToBigEndian64) }
        int64_t writeUint16(uint16_t value) { W(ToBigEndian16) }
        int64_t writeUint32(uint32_t value) { W(ToBigEndian32) }
        int64_t writeUint64(uint64_t value) { W(ToBigEndian64) }
#undef W
#define R(func)                             \
    auto len = read(&value, sizeof(value)); \
    value = func(value);                    \
    return len;
        int64_t readInt16(int16_t &value) { R(FromBigEndian16) }
        int64_t readInt32(int32_t &value) { R(FromBigEndian32) }
        int64_t readInt64(int64_t &value) { R(FromBigEndian64) }
        int64_t readUint16(uint16_t &value) { R(FromBigEndian16) }
        int64_t readUint32(uint32_t &value) { R(FromBigEndian32) }
        int64_t readUint64(uint64_t &value) { R(FromBigEndian64) }
#undef R

        void close() override;
        [[nodiscard]] const socket_t &getRawSocket() const { return handle; }
        [[nodiscard]] const Address::Ptr &getAddress() const { return address; }

    private:
        socket_t handle{};
        Address::Ptr address;
    };

}// namespace sese