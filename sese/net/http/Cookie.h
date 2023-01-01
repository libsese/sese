#pragma once
#include <sese/Config.h>

namespace sese::http {

    class API Cookie {
    public:
        using Ptr = std::shared_ptr<Cookie>;

        explicit Cookie(const std::string &name) noexcept;
        Cookie(const std::string &name, const std::string &value) noexcept;

    public:
        [[nodiscard]] bool expired() const;

        [[nodiscard]] bool isSecure() const;
        void setSecure(bool secure);
        [[nodiscard]] bool isHttpOnly() const;
        void setHttpOnly(bool httpOnly);
        [[nodiscard]] uint64_t getMaxAge() const;
        void setMaxAge(uint64_t maxAge);
        [[nodiscard]] uint64_t getExpires() const;
        void setExpires(uint64_t expires);
        [[nodiscard]] const std::string &getName() const;
        [[nodiscard]] const std::string &getValue() const;
        void setValue(const std::string &value);
        [[nodiscard]] const std::string &getDomain() const;
        void setDomain(const std::string &domain);
        [[nodiscard]] const std::string &getPath() const;
        void setPath(const std::string &path);

    private:
        bool secure = false;
        bool httpOnly = false;

        uint64_t maxAge = 0;
        uint64_t expires = 0;

        std::string name;
        std::string value;
        std::string domain;
        std::string path;
    };
}// namespace sese::http