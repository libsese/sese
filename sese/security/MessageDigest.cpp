#include "sese/security/MessageDigest.h"
#include "sese/security/evp/MD5Context.h"
#include "sese/security/evp/SHA1Context.h"
#include "sese/security/evp/SHA256Context.h"
#include "sese/security/evp/SHA384Context.h"
#include "sese/security/evp/SHA512Context.h"
#include "sese/util/MemoryViewer.h"

std::string sese::security::MessageDigest::digest(sese::security::MessageDigest::Type type, InputStream *input, bool is_cap) noexcept {
    switch (type) {
        // case Type::MD4: {
        //     auto str = std::unique_ptr<char[]>(new char[MD4_DIGEST_LENGTH * 2 + 1]);
        //     digestMD4(str.get(), input, isCap);
        //     return str;
        // }
        case Type::MD5: {
            char str[16 * 2 + 1]{};
            digestMD5(str, input, is_cap);
            return {str, 16 * 2};
        }
        case Type::SHA1: {
            char str[20 * 2 + 1]{};
            digestSH1(str, input, is_cap);
            return {str, 20 * 2};
        }
        // case Type::SHA224: {
        //     auto str = std::unique_ptr<char[]>(new char[SHA224_DIGEST_LENGTH * 2 + 1]);
        //     digestSHA224(str.get(), input, isCap);
        //     return str;
        // }
        case Type::SHA256: {
            char str[32 * 2 + 1]{};
            digestSHA256(str, input, is_cap);
            return {str, 32 * 2};
        }
        case Type::SHA384: {
            char str[48 * 2 + 1]{};
            digestSHA384(str, input, is_cap);
            return {str, 48 * 2};
        }
        case Type::SHA512: {
            char str[64 * 2 + 1];
            digestSHA512(str, input, is_cap);
            return {str, 64 * 2};
        }
        default:
            // Never reach
            return {};
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

void sese::security::MessageDigest::digestMD5(char *str, InputStream *input, bool is_cap) noexcept {
    evp::MD5Context context;
    size_t len;
    char buffer[1024];
    while ((len = input->read(buffer, 1024)) > 0) {
        context.update(buffer, len);
    }
    context.final();

    auto result = (uint8_t *) context.getResult();
    std::div_t div_rt;
    for (int i = 0; i < context.getLength(); ++i) {
        div_rt = std::div(result[i], 0x10);
        str[i * 2 + 0] = MemoryViewer::toChar(div_rt.quot, is_cap);
        str[i * 2 + 1] = MemoryViewer::toChar(div_rt.rem, is_cap);
    }
    str[32] = 0;
}

void sese::security::MessageDigest::digestSH1(char *str, InputStream *input, bool is_cap) noexcept {
    evp::SHA1Context context;
    size_t len;
    char buffer[1024];
    while ((len = input->read(buffer, 1024)) > 0) {
        context.update(buffer, len);
    }
    context.final();

    auto result = (uint8_t *) context.getResult();
    std::div_t div_rt;
    for (int i = 0; i < context.getLength(); ++i) {
        div_rt = std::div(result[i], 0x10);
        str[i * 2 + 0] = MemoryViewer::toChar(div_rt.quot, is_cap);
        str[i * 2 + 1] = MemoryViewer::toChar(div_rt.rem, is_cap);
    }
    str[40] = 0;
}

// void sese::security::MessageDigest::digestSHA224(char *str, InputStream *input, bool isCap) noexcept {
//     uint8_t result[SHA224_DIGEST_LENGTH]{0};
//     char buffer[1024];
//     // SHA256_CTX ctx;
//     // SHA224_Init(&ctx);
//     // size_t len;
//     // while ((len = input->read(buffer, 1024)) > 0) {
//     //     SHA224_Update(&ctx, buffer, len);
//     // }
//     // SHA224_Final(result, &ctx);
//
//     auto ctx = EVP_MD_CTX_new();
//     EVP_DigestInit(ctx, EVP_sha224());
//     size_t len;
//     while ((len = input->read(buffer, 1024)) > 0) {
//         EVP_DigestUpdate(ctx, buffer, len);
//     }
//     EVP_DigestFinal(ctx, result, nullptr);
//     EVP_MD_CTX_free(ctx);
//
//     std::div_t divRt;
//     for (int i = 0; i < SHA224_DIGEST_LENGTH; ++i) {
//         divRt = std::div(result[i], 0x10);
//         str[i * 2 + 0] = MemoryViewer::toChar(divRt.quot, isCap);
//         str[i * 2 + 1] = MemoryViewer::toChar(divRt.rem, isCap);
//     }
//     str[56] = 0;
// }

void sese::security::MessageDigest::digestSHA256(char *str, InputStream *input, bool is_cap) noexcept {
    evp::SHA256Context context;
    size_t len;
    char buffer[1024];
    while ((len = input->read(buffer, 1024)) > 0) {
        context.update(buffer, len);
    }
    context.final();

    auto result = (uint8_t *) context.getResult();
    std::div_t div_rt;
    for (int i = 0; i < context.getLength(); ++i) {
        div_rt = std::div(result[i], 0x10);
        str[i * 2 + 0] = MemoryViewer::toChar(div_rt.quot, is_cap);
        str[i * 2 + 1] = MemoryViewer::toChar(div_rt.rem, is_cap);
    }
    str[64] = 0;
}

void sese::security::MessageDigest::digestSHA384(char *str, InputStream *input, bool is_cap) noexcept {
    evp::SHA384Context context;
    size_t len;
    char buffer[1024];
    while ((len = input->read(buffer, 1024)) > 0) {
        context.update(buffer, len);
    }
    context.final();

    auto result = (uint8_t *) context.getResult();
    std::div_t div_rt;
    for (int i = 0; i < context.getLength(); ++i) {
        div_rt = std::div(result[i], 0x10);
        str[i * 2 + 0] = MemoryViewer::toChar(div_rt.quot, is_cap);
        str[i * 2 + 1] = MemoryViewer::toChar(div_rt.rem, is_cap);
    }
    str[96] = 0;
}

void sese::security::MessageDigest::digestSHA512(char *str, InputStream *input, bool is_cap) noexcept {
    evp::SHA512Context context;
    size_t len;
    char buffer[1024];
    while ((len = input->read(buffer, 1024)) > 0) {
        context.update(buffer, len);
    }
    context.final();

    auto result = (uint8_t *) context.getResult();
    std::div_t div_rt;
    for (int i = 0; i < context.getLength(); ++i) {
        div_rt = std::div(result[i], 0x10);
        str[i * 2 + 0] = MemoryViewer::toChar(div_rt.quot, is_cap);
        str[i * 2 + 1] = MemoryViewer::toChar(div_rt.rem, is_cap);
    }
    str[128] = 0;
}