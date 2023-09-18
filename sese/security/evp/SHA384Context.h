/**
* @file SHA384Context.h
* @author kaoru
* @version 0.1
* @brief EVP SHA384 算法上下文
* @date 2023年9月13日
*/

#pragma once

#include <sese/security/evp/Context.h>

namespace sese::security::evp {

    /// EVP SHA384 算法上下文
    class API SHA384Context : public Context {
    public:
        SHA384Context() noexcept;
        ~SHA384Context() noexcept override;
        void update(const void *buffer, size_t len) noexcept override;
        void final() noexcept override;
        void *getResult() noexcept override;
        size_t getLength() noexcept override;

    private:
        size_t length = 48;
        uint8_t result[48]{};
#ifdef SESE_PLATFORM_WINDOWS
        uint64_t hProv = 0;
        uint64_t hHash = 0;
#else
        void *context = nullptr;
#endif
    };
}// namespace sese::security::evp