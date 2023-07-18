#include "sese/convert/Base64Converter.h"
#include "sese/util/Endian.h"

#include <cmath>
#include <cstring>
#include <vector>
#include <algorithm>

using sese::Base64Converter;
using sese::Stream;

/// 标准 BASE64 码表
sese::Base64Converter::CodePage Base64CodePage = (sese::Base64Converter::CodePage) "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

sese::Base64Converter::CodePage Base62CodePage = (sese::Base64Converter::CodePage) "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

void inline encode(unsigned char in, unsigned char &out) {
    out = Base64CodePage[in];
}

void Base64Converter::encode(InputStream *src, OutputStream *dest) {
    unsigned char buffer[3]{0};
    unsigned char result[4]{0};

    int64_t len;
    while ((len = src->read(buffer, 3)) != 0) {
        if (len == 2) {
            buffer[2] = 0b00000000;
        } else if (len == 1) {
            buffer[1] = 0b00000000;
            buffer[2] = 0b00000000;
        }

        unsigned char bits = 0b00000000;
        // 1
        bits = (buffer[0] & 0b11111100) >> 2;
        ::encode(bits, result[0]);
        // 2
        bits = (buffer[0] & 0b00000011) << 4 | (buffer[1] & 0b11110000) >> 4;
        ::encode(bits, result[1]);
        // 3
        if (len >= 2) {
            bits = (buffer[1] & 0b00001111) << 2 | (buffer[2] & 0b11000000) >> 6;
            ::encode(bits, result[2]);

            // 4
            if (len == 3) {
                bits = (buffer[2] & 0b00111111);
                ::encode(bits, result[3]);
            }
        }
        dest->write(result, len + 1);
        if (len != 3) dest->write("==", len == 1 ? 2 : 1);
    }
}

// GCOVR_EXCL_START
void Base64Converter::encode(const InputStream::Ptr &src, const OutputStream::Ptr &dest) {
    encode(src.get(), dest.get());
}
// GCOVR_EXCL_STOP

void Base64Converter::decode(InputStream *src, OutputStream *dest) {
    unsigned char buffer[4]{0};

    int64_t len;
    while ((len = src->read(buffer, 4)) != 0) {
        for (auto i = 0; i < len; i++) {
            // 此处已是逻辑完备的
            // GCOVR_EXCL_START
            if (buffer[i] >= 'A' && buffer[i] <= 'Z') {
                buffer[i] -= 65;
            } else if (buffer[i] >= 'a' && buffer[i] <= 'z') {
                buffer[i] -= 71;
            } else if (buffer[i] >= '0' && buffer[i] <= '9') {
                buffer[i] += 4;
            } else if (buffer[i] == '+' || buffer[i] == '-') {
                buffer[i] = 62;
            } else if (buffer[i] == '/' || buffer[i] == '_') {
                buffer[i] = 63;
            } else {
                buffer[i] = 0;
            }
            // GCOVR_EXCL_STOP
        }

        // 1
        if (len >= 2) {
            buffer[0] = (buffer[0] << 2) | (buffer[1] & 0b00110000) >> 4;

            // 2
            if (len >= 3) {
                buffer[1] = (buffer[1] & 0b00001111) << 4 | (buffer[2] & 0b00111100) >> 2;

                // 3
                if (len == 4) {
                    buffer[2] = (buffer[2] & 0b00000011) << 6 | buffer[3];
                }
            }
        }

        dest->write(buffer, len - 1);
    }
}

// GCOVR_EXCL_START
void Base64Converter::decode(const InputStream::Ptr &src, const OutputStream::Ptr &dest) {
    decode(src.get(), dest.get());
}
// GCOVR_EXCL_STOP

bool Base64Converter::encodeInteger(size_t num, sese::OutputStream *output) noexcept {
    if (num == 0) {
        output->write(&Base62CodePage[0], 1);
        return true;
    }

    std::vector<unsigned char> vector;
    auto base = strlen((const char *) Base62CodePage);
    while (num > 0) {
        std::div_t res = div((int) num, (int) base);
        num = res.quot;
        vector.push_back(Base62CodePage[res.rem]);
    }

    std::reverse(vector.begin(), vector.end());
    return output->write(vector.data(), vector.size()) == vector.size();
}

int64_t Base64Converter::decodeBuffer(const unsigned char *buffer, size_t size) noexcept {
    auto base = strlen((const char *) Base62CodePage);
    auto page = std::string_view((const char *) Base62CodePage, base);
    int64_t num = 0;
    for (auto idx = 0; idx < size; ++idx) {
        auto power = (size - (idx + 1));
        auto i = page.find((char) (buffer[idx]));
        if (i == std::string::npos) return -1;
        num += i * static_cast<int64_t>(std::pow(base, power));// NOLINT
    }
    return num;
}

bool Base64Converter::encodeBase62(InputStream *input, OutputStream *output) noexcept {
    unsigned char buffer[4]{};

    int64_t len;
    while ((len = input->read(buffer + 1, 3)) != 0) {
        if (len == 2) {
            buffer[3] = buffer[2];
            buffer[2] = buffer[1];
            buffer[1] = 0;
        } else if (len == 1) {
            buffer[3] = buffer[1];
            buffer[2] = 0;
            buffer[1] = 0;
        }
        // buffer[0] always be zero
        auto num = *(uint32_t *) &buffer;
        num = FromBigEndian32(num);
        if (!encodeInteger(num, output)) {
            return false;
        }
    }

    return true;
}

bool Base64Converter::decodeBase62(InputStream *input, OutputStream *output) noexcept {
    unsigned char buffer[4]{};

    int64_t len;
    while ((len = input->read(buffer, 4)) != 0) {
        auto num = decodeBuffer(buffer, len);
        // GCOVR_EXCL_START
        if (num == -1) {
            return false;
        }

        num = ToBigEndian32(num);
        if (len == 4) {
            if (3 != output->write(((char *) &num) + 1, 3)) {
                return false;
            }
        } else if (len == 3) {
            if (2 != output->write(((char *) &num) + 2, 2)) {
                return false;
            }
        } else if (len == 2) {
            if (1 != output->write(((char *) &num) + 3, 1)) {
                return false;
            }
        } else {
            return false;
        }
        // GCOVR_EXCL_STOP

        memset(buffer, 0, 4);
    }

    return true;
}