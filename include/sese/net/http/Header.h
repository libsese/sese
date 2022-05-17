/**
 * @file Header.h
 * @author kaoru
 * @brief Http 头部键值集合
 * @date 2022年05月17日
 */
#pragma once
#include <sese/Config.h>
#include <map>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese::http {

    /// @brief Http 版本
    enum class HttpVersion {
        VERSION_1_1,
        VERSION_UNKNOWN
    };

    /**
     * Http 头部键值集合
     */
    class API Header {
    public:
        using Ptr = std::shared_ptr<Header>;
        using KeyValueType = std::pair<const std::string &, const std::string &>;

        explicit Header() = default;
        Header(const std::initializer_list<KeyValueType> &initializerList) noexcept;
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