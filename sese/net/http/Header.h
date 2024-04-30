/**
 * @file Header.h
 * @author kaoru
 * @brief HTTP 头部键值集合
 * @date 2022年05月17日
 */
#pragma once
#include <sese/net/http/CookieMap.h>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese::net::http {

/// @brief HTTP 版本
enum class HttpVersion {
    VERSION_1_1,
    VERSION_2,
    VERSION_UNKNOWN
};

/**
 * @brief HTTP 头部键值集合
 */
class API Header {
public:
    using Ptr = std::unique_ptr<Header>;
    using KeyValueType = std::pair<std::string, std::string>;

    explicit Header() = default;
    Header(const std::initializer_list<KeyValueType> &initializer_list) noexcept;
    virtual ~Header() = default;

    void set(const std::string &key, const std::string &value) noexcept;
    const std::string &get(const std::string &key, const std::string &default_value) noexcept;
#if SESE_CXX_STANDARD > 201700L
    std::string_view getView(const std::string &key, const std::string &default_value) noexcept;
#endif
    inline std::map<std::string, std::string>::iterator begin() noexcept { return headers.begin(); }
    inline std::map<std::string, std::string>::iterator end() noexcept { return headers.end(); }

    void clear() { headers.clear(); }
    [[nodiscard]] bool empty() const { return headers.empty(); }
    [[nodiscard]] size_t size() const { return headers.size(); }

    /// 判断某个字段是否存在
    /// @param key 头部字段名称
    /// @return 结果
    bool exist(const std::string &key) { return headers.find(key) != headers.end(); }
    /// 当确定一定存在此字段时可以调用
    /// @see sese::net::http::Header::exist
    /// @param key 头部字段名称
    /// @return 值
    const std::string &get(const std::string &key) { return headers.at(key); }

    /// 获取当前 Cookie 映射集
    /// \retval nullptr 当前映射集为空
    [[nodiscard]] const CookieMap::Ptr &getCookies() const;
    /// 设置当前 Cookie 映射集
    /// \param cookies 欲设置的映射集
    void setCookies(const CookieMap::Ptr &cookies);

    /// 通过名称获取 Cookie
    /// \param name Cookie 名称
    /// \retval nullptr Cookie 不存在
    [[nodiscard]] Cookie::Ptr getCookie(const std::string &name) const;
    /// 添加一个 Cookie
    /// \param cookie 欲添加的 Cookie
    void setCookie(const Cookie::Ptr &cookie);

protected:
    std::map<std::string, std::string> headers;
    CookieMap::Ptr cookies = nullptr;
};
} // namespace sese::net::http