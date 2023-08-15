#include <sese/security/evp/Context.h>

namespace sese::security::evp {
    class API MD5Context : public Context {
    public:
        MD5Context() noexcept;
        ~MD5Context() noexcept override;
        void update(const void *buffer, size_t len) noexcept override;
        void final() noexcept override;
        void *getResult() noexcept override;
        size_t getLength() noexcept override;

    private:
        size_t length = 16;
        uint8_t result[16]{};
#ifdef SESE_PLATFORM_WINDOWS
        uint64_t hProv = 0;
        uint64_t hHash = 0;
#else
        void *context = nullptr;
#endif
    };
}// namespace sese::security::evp