#pragma once
#include <sese/security/SecuritySocket.h>

namespace sese::security {

    class API SSLContext final : public std::enable_shared_from_this<SSLContext> {
    public:
        using Ptr = std::shared_ptr<SSLContext>;

        enum FileType {
            ASN1,
            PEM
        };

        explicit SSLContext(const void *method) noexcept;
        ~SSLContext() noexcept;

        [[nodiscard]] void *getContext() const noexcept;

        bool importCertFile(const char *file, FileType type = PEM) noexcept;
        bool importPrivateKey(const char *file, FileType type = PEM) noexcept;
        bool authPrivateKey() noexcept;

        bool verifyAndLoad(const char *file) noexcept;

        Socket::Ptr newSocketPtr(Socket::Family family, int32_t flags);

    private:
        // SSL_CTX *context = nullptr;
        void *context = nullptr;
    };
}