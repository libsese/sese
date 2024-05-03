/// \file SM3Context.h
/// \brief EVP SM3 算法上下文
/// \author kaoru
/// \date 2024年04月11日

#pragma once

#include <sese/security/evp/Context.h>

namespace sese::security::evp {

/// EVP SM3 算法上下文
class SM3Context : public Context {
public:
    SM3Context() noexcept;
    ~SM3Context() noexcept override;
    void update(const void *buffer, size_t len) noexcept override;
    void final() noexcept override;
    void *getResult() noexcept override;
    size_t getLength() noexcept override;

private:
    size_t length = 32;
    uint8_t result[32]{};
    void *context = nullptr;
};

}