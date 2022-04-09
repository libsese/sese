#pragma once
#include "IPv6Address.h"
#include "Config.h"
#include "Stream.h"

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese {

    class API Socket : public Stream, std::enable_shared_from_this<Socket> {
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
            Read = 0,
            Write = 1,
            Both = 2
        };

    public:
        Socket(Family family, Type type, int32_t protocol = IPPROTO_IP) noexcept;
        ~Socket() noexcept;

    public:
        int32_t bind(Address::Ptr addr) noexcept;
        int32_t connect(Address::Ptr addr) noexcept;
        int32_t listen(int32_t backlog) const noexcept;
        Socket::Ptr accept() const;
        int32_t shutdown(ShutdownMode mode) const;

        int64_t read(void *buffer, size_t length) override;
        int64_t write(void *buffer, size_t length) override;
        void close() override;

        const socket_t &getRawSocket() const { return handle; }
        const Address::Ptr &getAddress() const { return address; }

    protected:
        Socket(socket_t handle, Address::Ptr address) noexcept;

    private:
        socket_t handle{};
        Address::Ptr address;
    };

}// namespace sese