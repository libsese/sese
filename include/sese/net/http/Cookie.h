#pragma once
#include <sese/Config.h>

namespace sese::http {

    class API Cookie {
    public:
        Cookie(const std::string &name);
        Cookie(const std::string &name, const std::string &value);
        Cookie(const std::string &name, const std::string &value, const std::string &path);
        Cookie(const std::string &name, const std::string &value, const std::string &path, const std::string &domain);

    public:
        bool expired() const;

        bool isSecure() const;
        void setSecure(bool secure);
        bool isDiscard() const;
        void setDiscard(bool discard);
        bool isHttpOnly() const;
        void setHttpOnly(bool httpOnly);
        int32_t getVersion() const;
        void setVersion(int32_t version);
        int64_t getExpires() const;
        void setExpires(int64_t expires);
        int64_t getTimestamp() const;
        void setTimestamp(int64_t timestamp);
        const std::string &getName() const;
        void setName(const std::string &name);
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
        bool discard = false;
        bool httpOnly = false;

        int32_t version = 0;
        int64_t expires = 0;
        int64_t timestamp = 0;

        std::string name;
        std::string value;
        std::string domain;
        std::string path;

        std::string comment;
        std::string commentUrl;
    };
}// namespace sese::http