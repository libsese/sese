/// \file Requestable.h
/// \brief 可请求接口
/// \author kaoru
/// \date 2024年02月23日

#pragma once

#include <sese/net/http/Request.h>
#include <sese/net/http/Response.h>
#include <sese/io/InputStream.h>
#include <sese/io/OutputStream.h>

namespace sese::net::http {
/// 可请求接口
class Requestable : public io::InputStream, public io::OutputStream {
public:
    ~Requestable() override = default;

    /// 初始化函数
    /// \param url 服务地址，支持 https/http
    /// \param proxy 代理地址，支持 https/http，为空则不使用代理
    /// \return 初始化结果
    virtual bool init(const std::string &url, const std::string &proxy) = 0;

    /// 执行请求
    /// \return 请求状态
    virtual bool request() = 0;

    /// 获取最后一次错误
    /// \return 最后一次错误
    virtual int getLastError() = 0;

    /// 获取最后一次错误信息
    /// \return 最后一次错误信息
    virtual std::string getLastErrorString() = 0;

    int64_t read(void *buf, size_t len) override = 0;

    int64_t write(const void *buf, size_t len) override = 0;

    /// 获取请求头集合
    /// \return 请求头集合
    Request::Ptr &getRequest() { return req; }

    /// 获取响应头集合
    /// \return 响应头集合
    Response::Ptr &getResponse() { return resp; }

protected:
    Request::Ptr req = nullptr;
    Response::Ptr resp = nullptr;
};
} // namespace sese::net::http