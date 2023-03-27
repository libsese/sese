#include "sese/security/MessageDigest.h"
#include "sese/util/MemoryViewer.h"

#include "openssl/md4.h"
#include "openssl/md5.h"
#include "openssl/sha.h"
#include <openssl/evp.h>
#include <openssl/types.h>

std::unique_ptr<char[]> sese::security::MessageDigest::digest(sese::security::MessageDigest::Type type, InputStream *input, bool isCap) noexcept {
    switch (type) {
        // case Type::MD4: {
        //     auto str = std::unique_ptr<char[]>(new char[MD4_DIGEST_LENGTH * 2 + 1]);
        //     digestMD4(str.get(), input, isCap);
        //     return str;
        // }
        case Type::MD5: {
            auto str = std::unique_ptr<char[]>(new char[MD5_DIGEST_LENGTH * 2 + 1]);
            digestMD5(str.get(), input, isCap);
            return str;
        }
        case Type::SHA1: {
            auto str = std::unique_ptr<char[]>(new char[SHA_DIGEST_LENGTH * 2 + 1]);
            digestSH1(str.get(), input, isCap);
            return str;
        }
        case Type::SHA224: {
            auto str = std::unique_ptr<char[]>(new char[SHA224_DIGEST_LENGTH * 2 + 1]);
            digestSHA224(str.get(), input, isCap);
            return str;
        }
        case Type::SHA256: {
            auto str = std::unique_ptr<char[]>(new char[SHA256_DIGEST_LENGTH * 2 + 1]);
            digestSHA256(str.get(), input, isCap);
            return str;
        }
        case Type::SHA384: {
            auto str = std::unique_ptr<char[]>(new char[SHA384_DIGEST_LENGTH * 2 + 1]);
            digestSHA384(str.get(), input, isCap);
            return str;
        }
        case Type::SHA512: {
            auto str = std::unique_ptr<char[]>(new char[SHA512_DIGEST_LENGTH * 2 + 1]);
            digestSHA512(str.get(), input, isCap);
            return str;
        }
        default:
            // Never reach
            return nullptr;
    }
}

// void sese::security::MessageDigest::digestMD4(char *str, InputStream *input, bool isCap) noexcept {
//     uint8_t result[MD4_DIGEST_LENGTH]{0};
//     char buffer[1024];
//     MD4_CTX ctx;
//     MD4_Init(&ctx);
//     size_t len;
//     while ((len = input->read(buffer, 1024)) > 0) {
//         MD4_Update(&ctx, buffer, len);
//     }
//     MD4_Final(result, &ctx);

//     std::div_t divRt;
//     for (int i = 0; i < 16; ++i) {
//         divRt = std::div(result[i], 0x10);
//         str[i * 2 + 0] = MemoryViewer::toChar(divRt.quot, isCap);
//         str[i * 2 + 1] = MemoryViewer::toChar(divRt.rem, isCap);
//     }
//     str[32] = 0;
// }

void sese::security::MessageDigest::digestMD5(char *str, InputStream *input, bool isCap) noexcept {
    uint8_t result[MD5_DIGEST_LENGTH]{0};
    char buffer[1024];
    // MD5_CTX ctx;
    // MD5_Init(&ctx);
    // size_t len;
    // while ((len = input->read(buffer, 1024)) > 0) {
    //     MD5_Update(&ctx, buffer, len);
    // }
    // MD5_Final(result, &ctx);

    auto ctx = EVP_MD_CTX_new();
    EVP_DigestInit(ctx, EVP_md5());
    size_t len;
    while ((len = input->read(buffer, 1024)) > 0) {
        EVP_DigestUpdate(ctx, buffer, len);
    }
    EVP_DigestFinal(ctx, result, nullptr);
    EVP_MD_CTX_free(ctx);

    std::div_t divRt;
    for (int i = 0; i < 16; ++i) {
        divRt = std::div(result[i], 0x10);
        str[i * 2 + 0] = MemoryViewer::toChar(divRt.quot, isCap);
        str[i * 2 + 1] = MemoryViewer::toChar(divRt.rem, isCap);
    }
    str[32] = 0;
}

void sese::security::MessageDigest::digestSH1(char *str, InputStream *input, bool isCap) noexcept {
    uint8_t result[SHA_DIGEST_LENGTH]{0};
    char buffer[1024];
    // SHA_CTX ctx;
    // SHA1_Init(&ctx);
    // size_t len;
    // while ((len = input->read(buffer, 1024)) > 0) {
    //     SHA1_Update(&ctx, buffer, len);
    // }
    // SHA1_Final(result, &ctx);

    auto ctx = EVP_MD_CTX_new();
    EVP_DigestInit(ctx, EVP_sha1());
    size_t len;
    while ((len = input->read(buffer, 1024)) > 0) {
        EVP_DigestUpdate(ctx, buffer, len);
    }
    EVP_DigestFinal(ctx, result, nullptr);
    EVP_MD_CTX_free(ctx);

    std::div_t divRt;
    for (int i = 0; i < 20; ++i) {
        divRt = std::div(result[i], 0x10);
        str[i * 2 + 0] = MemoryViewer::toChar(divRt.quot, isCap);
        str[i * 2 + 1] = MemoryViewer::toChar(divRt.rem, isCap);
    }
    str[40] = 0;
}

void sese::security::MessageDigest::digestSHA224(char *str, InputStream *input, bool isCap) noexcept {
    uint8_t result[SHA224_DIGEST_LENGTH]{0};
    char buffer[1024];
    // SHA256_CTX ctx;
    // SHA224_Init(&ctx);
    // size_t len;
    // while ((len = input->read(buffer, 1024)) > 0) {
    //     SHA224_Update(&ctx, buffer, len);
    // }
    // SHA224_Final(result, &ctx);

    auto ctx = EVP_MD_CTX_new();
    EVP_DigestInit(ctx, EVP_sha224());
    size_t len;
    while ((len = input->read(buffer, 1024)) > 0) {
        EVP_DigestUpdate(ctx, buffer, len);
    }
    EVP_DigestFinal(ctx, result, nullptr);
    EVP_MD_CTX_free(ctx);

    std::div_t divRt;
    for (int i = 0; i < SHA224_DIGEST_LENGTH; ++i) {
        divRt = std::div(result[i], 0x10);
        str[i * 2 + 0] = MemoryViewer::toChar(divRt.quot, isCap);
        str[i * 2 + 1] = MemoryViewer::toChar(divRt.rem, isCap);
    }
    str[56] = 0;
}

void sese::security::MessageDigest::digestSHA256(char *str, InputStream *input, bool isCap) noexcept {
    uint8_t result[SHA256_DIGEST_LENGTH]{0};
    char buffer[1024];
    // SHA256_CTX ctx;
    // SHA256_Init(&ctx);
    // size_t len;
    // while ((len = input->read(buffer, 1024)) > 0) {
    //     SHA256_Update(&ctx, buffer, len);
    // }
    // SHA256_Final(result, &ctx);

    auto ctx = EVP_MD_CTX_new();
    EVP_DigestInit(ctx, EVP_sha256());
    size_t len;
    while ((len = input->read(buffer, 1024)) > 0) {
        EVP_DigestUpdate(ctx, buffer, len);
    }
    EVP_DigestFinal(ctx, result, nullptr);
    EVP_MD_CTX_free(ctx);

    std::div_t divRt;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        divRt = std::div(result[i], 0x10);
        str[i * 2 + 0] = MemoryViewer::toChar(divRt.quot, isCap);
        str[i * 2 + 1] = MemoryViewer::toChar(divRt.rem, isCap);
    }
    str[64] = 0;
}

void sese::security::MessageDigest::digestSHA384(char *str, InputStream *input, bool isCap) noexcept {
    uint8_t result[SHA384_DIGEST_LENGTH]{0};
    char buffer[1024];
    // SHA512_CTX ctx;
    // SHA384_Init(&ctx);
    // size_t len;
    // while ((len = input->read(buffer, 1024)) > 0) {
    //     SHA384_Update(&ctx, buffer, len);
    // }
    // SHA384_Final(result, &ctx);

    auto ctx = EVP_MD_CTX_new();
    EVP_DigestInit(ctx, EVP_sha384());
    size_t len;
    while ((len = input->read(buffer, 1024)) > 0) {
        EVP_DigestUpdate(ctx, buffer, len);
    }
    EVP_DigestFinal(ctx, result, nullptr);
    EVP_MD_CTX_free(ctx);

    std::div_t divRt;
    for (int i = 0; i < SHA384_DIGEST_LENGTH; ++i) {
        divRt = std::div(result[i], 0x10);
        str[i * 2 + 0] = MemoryViewer::toChar(divRt.quot, isCap);
        str[i * 2 + 1] = MemoryViewer::toChar(divRt.rem, isCap);
    }
    str[96] = 0;
}

void sese::security::MessageDigest::digestSHA512(char *str, InputStream *input, bool isCap) noexcept {
    uint8_t result[SHA512_DIGEST_LENGTH]{0};
    char buffer[1024];
    // SHA512_CTX ctx;
    // SHA512_Init(&ctx);
    // size_t len;
    // while ((len = input->read(buffer, 1024)) > 0) {
    //     SHA512_Update(&ctx, buffer, len);
    // }
    // SHA512_Final(result, &ctx);

    auto ctx = EVP_MD_CTX_new();
    EVP_DigestInit(ctx, EVP_sha512());
    size_t len;
    while ((len = input->read(buffer, 1024)) > 0) {
        EVP_DigestUpdate(ctx, buffer, len);
    }
    EVP_DigestFinal(ctx, result, nullptr);
    EVP_MD_CTX_free(ctx);

    std::div_t divRt;
    for (int i = 0; i < SHA512_DIGEST_LENGTH; ++i) {
        divRt = std::div(result[i], 0x10);
        str[i * 2 + 0] = MemoryViewer::toChar(divRt.quot, isCap);
        str[i * 2 + 1] = MemoryViewer::toChar(divRt.rem, isCap);
    }
    str[128] = 0;
}