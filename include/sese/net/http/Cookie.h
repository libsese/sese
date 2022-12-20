#pragma once
#include <sese/Config.h>

namespace sese::http {

    class API Cookie {
    public:
        using Ptr = std::shared_ptr<Cookie>;

        explicit Cookie(const std::string &name) noexcept;

    public:
        bool expired() const;

        bool isSecure() const;
        void setSecure(bool secure);
        bool isHttpOnly() const;
        void setHttpOnly(bool httpOnly);
        int32_t getVersion() const;
        void setVersion(int32_t version);
        int64_t getExpires() const;
        void setExpires(int64_t expires);
        const std::string &getName() const;
        const std::string &getValue() const;
        void setValue(const std::string &value);
        const std::string &getDomain() const;
        void setDomain(const std::string &domain);
        const std::string &getPath() const;
        void setPath(const std::string &path);
        const std::string &getComment() const;
        void setComment(const std::string &comment);
        const std::string &getCommentUrl() const;
        void setCommentUrl(const std::string &commentUrl);

    private:
        bool secure = false;
        bool httpOnly = false;

        int32_t version = 0;
        int64_t expires = 0;

        std::string name;
        std::string value;
        std::string domain;
        std::string path;

        std::string comment;
        std::string commentUrl;
    };
}// namespace sese::http