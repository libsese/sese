#include "sese/io/OutputUtil.h"

int64_t operator<<(sese::io::OutputStream &out, const sese::text::StringView &data) noexcept {
    return out.write(data.data(), data.size());
}

int64_t operator<<(sese::io::OutputStream *out, const sese::text::StringView &data) noexcept {
    return out->write(data.data(), data.size());
}

int64_t operator<<(const std::unique_ptr<sese::io::OutputStream> &out, const sese::text::StringView &data) noexcept {
    return out->write(data.data(), data.size());
}

int64_t operator<<(const std::shared_ptr<sese::io::OutputStream> &out, const sese::text::StringView &data) noexcept {
    return out->write(data.data(), data.size());
}