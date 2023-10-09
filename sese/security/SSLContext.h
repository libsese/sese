/// \file SSLContext.h
/// \brief SSL 上下文
/// \version 0.1
/// \author kaoru
/// \date 2023年7月25日

#pragma once

#include <sese/security/SecuritySocket.h>

namespace sese::security {

/// SSL 上下文
class API SSLContext final : public std::enable_shared_from_this<SSLContext> {
public:
    using Ptr = std::shared_ptr<SSLContext>;
    using Socket = sese::net::Socket;

    explicit SSLContext(const void *method) noexcept;
    ~SSLContext() noexcept;

    [[nodiscard]] void *getContext() const noexcept;

    /// @brief 从文件中加载证书
    /// @param file 证书文件路径
    /// @return 加载结果
    bool importCertFile(const char *file) noexcept;
    /// @brief 从文件中加载私钥
    /// @param file 私钥文件路径
    /// @return 加载结果
    bool importPrivateKeyFile(const char *file) noexcept;
    /// @brief 校验证书和私钥
    /// @return 校验结果
    bool authPrivateKey() noexcept;

    // bool verifyAndLoad(const char *file) noexcept;

    /// @brief 从当前上下文中创建一个 TCP Socket
    /// @param family 协议
    /// @param flags 标志
    /// @return 创建完成的 Socket
    Socket::Ptr newSocketPtr(Socket::Family family, int32_t flags);

private:
    // SSL_CTX *context = nullptr;
    void *context = nullptr;
};
} // namespace sese::security