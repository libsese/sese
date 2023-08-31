#include "sese/util/OutputUtil.h"

int64_t operator<<(sese::OutputStream &out, const sese::text::StringView &data) noexcept {
    return out.write(data.data(), data.size());
}

int64_t operator<<(sese::OutputStream *out, const sese::text::StringView &data) noexcept {
    return out->write(data.data(), data.size());
}

int64_t operator<<(const std::unique_ptr<sese::OutputStream> &out, const sese::text::StringView &data) noexcept {
    return out->write(data.data(), data.size());
}

int64_t operator<<(const std::shared_ptr<sese::OutputStream> &out, const sese::text::StringView &data) noexcept {
    return out->write(data.data(), data.size());
}