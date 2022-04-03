#include "Address.h"

namespace sese {

    class API IPAddress : public Address {
    public:
        using Ptr = std::shared_ptr<IPAddress>;

        static Ptr create(const char *address, uint16_t port = 0);

    public:
        [[nodiscard]] virtual Ptr getBroadcastAddress(uint32_t prefixLen) const noexcept = 0;
        [[nodiscard]] virtual Ptr getNetworkAddress(uint32_t prefixLen) const noexcept = 0;
        [[nodiscard]] virtual Ptr getSubnetMask(uint32_t prefixLen) const noexcept = 0;

    private:
        uint16_t port = 0;
    };

}// namespace sese