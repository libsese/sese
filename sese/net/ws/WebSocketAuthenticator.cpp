#include "sese/net/ws/WebsocketAuthenticator.h"
#include "sese/util/Random.h"
#include "sese/io/InputBufferWrapper.h"
#include "sese/io/OutputBufferWrapper.h"
#include "sese/convert/Base64Converter.h"
#include "sese/convert/SHA1Util.h"

#include <random>

using sese::io::InputBufferWrapper;
using sese::io::OutputBufferWrapper;

const char *sese::net::ws::WebsocketAuthenticator::APPEND_STRING = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

void sese::net::ws::WebsocketAuthenticator::generateKey(uint8_t *key) {
    auto dev = std::random_device();
    uint64_t r = static_cast<decltype(r)>(dev());
    auto p = (uint8_t *) &r;
    for (int i = 0; i < 8; ++i) {
        key[i] = *(p + i);
    }
    r = static_cast<decltype(r)>(dev());
    for (int i = 0; i < 8; ++i) {
        key[i + 8] = *(p + i);
    }
}

std::pair<std::unique_ptr<char[]>, std::unique_ptr<char[]>> sese::net::ws::WebsocketAuthenticator::generateKeyPair() noexcept {
    // 生成二进制 key
    uint8_t buffer[16];
    generateKey(buffer);
    // 生成字符串 key
    auto keyString = std::unique_ptr<char[]>(new char[25]);
    keyString.get()[24] = 0;
    {
        auto in = InputBufferWrapper((const char *) buffer, 16);
        auto out = OutputBufferWrapper((char *) keyString.get(), 24);
        Base64Converter::encode(&in, &out);
    }

    // 拼接 key
    char buffer0[24 + 36];
    memcpy(buffer0 + 0, keyString.get(), 24);
    memcpy(buffer0 + 24, APPEND_STRING, 36);

    char buffer1[20];
    {
        auto in = InputBufferWrapper(buffer0, 60);
        auto out = OutputBufferWrapper(buffer1, 20);
        SHA1Util::encode(&in, &out);
    }

    auto resultString = std::unique_ptr<char[]>(new char[29]);
    resultString.get()[28] = 0;
    {
        auto in = InputBufferWrapper(buffer1, 20);
        auto out = OutputBufferWrapper(resultString.get(), 28);
        Base64Converter::encode(&in, &out);
    }

    return {std::move(keyString), std::move(resultString)};
}

bool sese::net::ws::WebsocketAuthenticator::verify(const char *key, const char *result) noexcept {
    // 拼接 key
    char buffer0[24 + 36];
    memcpy(buffer0 + 0, key, 24);
    memcpy(buffer0 + 24, APPEND_STRING, 36);

    char buffer1[20];
    {
        auto in = InputBufferWrapper(buffer0, 60);
        auto out = OutputBufferWrapper(buffer1, 20);
        SHA1Util::encode(&in, &out);
    }

    char buffer2[29];
    buffer2[28] = 0;
    {
        auto in = InputBufferWrapper(buffer1, 20);
        auto out = OutputBufferWrapper(buffer2, 28);
        Base64Converter::encode(&in, &out);
    }

    return 0 == strcmp(result, buffer2);
}

std::unique_ptr<char[]> sese::net::ws::WebsocketAuthenticator::toResult(const char *key) noexcept {
    // 拼接 key
    char buffer0[24 + 36];
    memcpy(buffer0 + 0, key, 24);
    memcpy(buffer0 + 24, APPEND_STRING, 36);

    char buffer1[20];
    {
        auto in = InputBufferWrapper(buffer0, 60);
        auto out = OutputBufferWrapper(buffer1, 20);
        SHA1Util::encode(&in, &out);
    }

    auto res = std::unique_ptr<char[]>(new char[29]);
    res.get()[28] = 0;
    {
        auto in = InputBufferWrapper(buffer1, 20);
        auto out = OutputBufferWrapper(res.get(), 28);
        Base64Converter::encode(&in, &out);
    }
    return std::move(res);
}
