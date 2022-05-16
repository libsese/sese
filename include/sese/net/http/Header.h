#pragma once
#include <sese/Config.h>
#include <map>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese::http {

    /**
     * Http 头部键值集合
     */
    class API Header {
    public:
        explicit Header() = default;
        Header(std::initializer_list<std::pair<const std::string &, const std::string &>> initializerList) noexcept;
        virtual ~Header() = default;

        void set(const std::string &key, const std::string &value) noexcept;
        const std::string &get(const std::string &key, const std::string &defaultValue) noexcept;
        std::string_view getView(const std::string &key, const std::string &defaultValue) noexcept;

        inline std::map<std::string, std::string>::iterator begin() noexcept { return headers.begin(); }
        inline std::map<std::string, std::string>::iterator end() noexcept { return headers.end(); }

    protected:
        std::map<std::string, std::string> headers;
    };
}// namespace sese