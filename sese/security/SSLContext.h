#pragma once
#include <sese/security/SecuritySocket.h>

namespace sese::security {

    class API SSLContext final : public std::enable_shared_from_this<SSLContext> {
    public:
        using Ptr = std::shared_ptr<SSLContext>;

        explicit SSLContext(const void *method) noexcept;
        ~SSLContext() noexcept;

        [[nodiscard]] void *getContext() const noexcept;

        bool importCertFile(const char *file, int type) noexcept;
        bool importPrivateKey(const char *file, int type) noexcept;
        bool authPrivateKey() noexcept;

        bool verifyAndLoad(const char *file) noexcept;

        Socket::Ptr newSocketPtr(Socket::Family family, int32_t flags);

    private:
        // SSL_CTX *context = nullptr;
        void *context = nullptr;
    };
}