#pragma once

#include <sese/io/InputBuffer.h>

namespace sese::security::evp {

/// EVP 加解密上下文
class Crypter {
public:
    virtual ~Crypter() = default;

    /// 处理数据
    /// @param out 输出缓冲区指针
    /// @param out_len 输出缓存的字节数
    /// @param in 输入缓冲区指针
    /// @param in_len 输入缓存区的大小
    /// @return 返回处理的字节数，失败返回负数
    virtual int update(void *out, int &out_len, const void *in, int in_len) const noexcept = 0;

    /// 处理最后的数据
    /// @param out 输出缓冲区指针
    /// @param out_len 输出缓存的字节数
    /// @return 返回处理的字节数，失败返回负数
    virtual int final(void *out, int &out_len) const noexcept = 0;
};

struct CrypterContext {
    using Ptr = std::unique_ptr<CrypterContext>;

    io::InputBuffer key;
    io::InputBuffer vector;
    /// 加密器指针，请勿手动修改该属性
    void *crypter_pointer{};
};

/// 为现有的加密器上下文选择加密器并验证密钥和向量是否合规
/// @todo 待实现以及补充文档
/// @param algorithm 算法配置选项
/// @param crypter_context 欲初始化的加密器上下文
/// @return 是否初始化成功
bool initCrypterContext(const std::string &algorithm, const CrypterContext::Ptr &crypter_context);

/// @brief 解密器
class Decrypter final : public Crypter {
public:
    explicit Decrypter(const CrypterContext::Ptr &crypter_context);

    ~Decrypter() override;

    int update(void *out, int &out_len, const void *in, int in_len) const noexcept override;

    int final(void *out, int &out_len) const noexcept override;

private:
    void *ctx_;
    const CrypterContext::Ptr &crypter_context_;
};

/// @brief 加密器
class Encrypter final : public Crypter {
public:
    explicit Encrypter(const CrypterContext::Ptr &crypter_context);

    ~Encrypter() override;

    int update(void *out, int &out_len, const void *in, int in_len) const noexcept override;

    int final(void *out, int &out_len) const noexcept override;

private:
    void *ctx_;
    const CrypterContext::Ptr &crypter_context_;
};

}