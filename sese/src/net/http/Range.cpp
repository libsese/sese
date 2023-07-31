#include "sese/net/http/Range.h"
#include "sese/text/StringBuilder.h"

sese::net::http::Range::Range(size_t begin, size_t len) noexcept {
    this->begin = begin;
    this->len = len;
}

std::vector<sese::net::http::Range> sese::net::http::Range::parse(const std::string &str, size_t total) noexcept {
    std::vector<sese::net::http::Range> vector;
    auto tmp0 = sese::text::StringBuilder::split(str, "=");
    if (tmp0.size() != 2) {
        return vector;
    }

    if (tmp0[0] != "bytes") {
        return vector;
    }

    auto tmp1 = sese::text::StringBuilder::split(tmp0[1], ", ");
    if (tmp1.empty()) {
        return vector;
    }

    if (tmp1.size() > 16) {
        return vector;
    }

    for (decltype(auto) item: tmp1) {
        auto tmp2 = sese::text::StringBuilder::split(item, "-");
        if (tmp2.size() == 1) {
            char *endPtr;
            auto begin = std::strtol(tmp2[0].c_str(), &endPtr, 10);
            if (begin > total - 1) {
                vector.clear();
                break;
            }
            vector.emplace_back(begin, total - begin);
        } else if (tmp2.size() == 2) {
            char *endPtr;
            auto begin = std::strtol(tmp2[0].c_str(), &endPtr, 10);
            auto end = std::strtol(tmp2[1].c_str(), &endPtr, 10);
            if (begin > total - 1 || end > total - 1) {
                vector.clear();
                break;
            }
            if (begin > end) {
                vector.clear();
                break;
            }
            vector.emplace_back(begin, end - begin + 1);
        } else {
            vector.clear();
            break;
        }
    }

    return vector;
}

std::string sese::net::http::Range::toString(size_t total) const noexcept {
    return "bytes " + std::to_string(begin) + "-" + std::to_string(begin + len - 1) + "/" + std::to_string(total);
}