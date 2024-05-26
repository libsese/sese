/// \file RequestableFactory.h
/// \brief 可请求类型工厂
/// \author kaoru
/// \date 2024年02月23日

#pragma once

#include <sese/net/http/Requestable.h>
#include <sese/util/NotInstantiable.h>

namespace sese::net::http {
/// 可请求类型工厂
class RequestableFactory final : public NotInstantiable {
public:
    RequestableFactory() = delete;

    /// 创建普通可请求类型
    /// \param url 请求地址，支持 https/http 协议
    /// \param proxy 代理地址，支持 https/http 协议，为空则不使用代理
    /// \return 可请求类型，创建失败返回 nullptr
    static std::unique_ptr<Requestable> createHttpRequest(const std::string &url, const std::string &proxy = "");
};
} // namespace sese::net::http