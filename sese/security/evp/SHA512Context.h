/**
* @file SHA512Context.h
* @author kaoru
* @version 0.1
* @brief EVP SHA512 算法上下文
* @date 2023年9月13日
*/

#pragma once

#include <sese/security/evp/Context.h>

namespace sese::security::evp {

/// EVP SHA512 算法上下文
class API SHA512Context : public Context {
public:
    SHA512Context() noexcept;
    ~SHA512Context() noexcept override;
    void update(const void *buffer, size_t len) noexcept override;
    void final() noexcept override;
    void *getResult() noexcept override;
    size_t getLength() noexcept override;

private:
    size_t length = 64;
    uint8_t result[64]{};
#ifdef SESE_PLATFORM_WINDOWS
    uint64_t hProv = 0;
    uint64_t hHash = 0;
#else
    void *context = nullptr;
#endif
};
} // namespace sese::security::evp