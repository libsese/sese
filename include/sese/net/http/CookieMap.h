/**
 * \file CookieMap.h
 * \brief Cookie 映射集合类
 * \date 2022.12.20
 * \author kaoru
 * \version 0.1
 */
#pragma once
#include <sese/net/http/Cookie.h>
#include <map>

namespace sese::http {

    /// \brief Cookie 映射集合类
    class API CookieMap {
    public:
        using Ptr = std::shared_ptr<CookieMap>;
        using Map = std::map<std::string, Cookie::Ptr>;

        /// 添加一个 Cookie
        /// \param cookie 欲添加的 Cookie
        void add(const Cookie::Ptr &cookie) noexcept;
        /// 获取一个现有的 Cookie
        /// \param name Cookie 名称
        /// \retval nullptr 不存在指定名称的 Cookie
        Cookie::Ptr find(const std::string &name) noexcept;

        inline Map::iterator begin() noexcept { return map.begin(); }
        inline Map::iterator end() noexcept { return map.end(); }

    private:
        Map map;
    };
}