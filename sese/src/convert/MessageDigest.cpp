#include <sese/convert/MessageDigest.h>
#include <sese/convert/MD5Util.h>
#include <sese/convert/SHA1Util.h>
#include <sese/convert/SHA256Util.h>

std::unique_ptr<char[]> sese::MessageDigest::digest(Type type, const InputStream::Ptr &source, bool isCap) noexcept {
    switch (type) {
        case MD5:
            return sese::MD5Util::encode(source, isCap);
        case SHA1:
            return sese::SHA1Util::encode(source, isCap);
        default:
            return sese::SHA256Util::encode(source, isCap);
    }
}

std::unique_ptr<char[]> sese::MessageDigest::digest(Type type, InputStream *source, bool isCap) noexcept {
    switch (type) {
        case MD5:
            return sese::MD5Util::encode(source, isCap);
        case SHA1:
            return sese::SHA1Util::encode(source, isCap);
        default:
            return sese::SHA256Util::encode(source, isCap);
    }
}