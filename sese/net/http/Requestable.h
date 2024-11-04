// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/// \file Requestable.h
/// \brief 可请求接口
/// \author kaoru
/// \date 2024年02月23日

#pragma once

#include <sese/net/http/Request.h>
#include <sese/net/http/Response.h>
#include <sese/io/InputStream.h>
#include <sese/io/PeekableStream.h>
#include <sese/io/OutputStream.h>

#include <functional>

namespace sese::net::http {
/// 可请求接口
class Requestable : public io::InputStream, public io::OutputStream {
public:
    using WriteCallback = std::function<int64_t(const void *, size_t)>;
    using ReadCallback = std::function<int64_t(void *, size_t)>;

    ~Requestable() override = default;

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

    /// 设置请求 body 外部来源，此选项将在请求完成后重置
    /// @param read_data body 读取来源
    /// @param expect_total body 预计大小
    void setReadData(io::PeekableStream *read_data, size_t expect_total) {
        this->read_data = read_data;
        this->expect_total = expect_total;
    }

    /// 设置响应 body 外部目的地，此选项将在请求完成后重置
    /// @param write_data 接收 body 目的地
    void setWriteData(io::OutputStream *write_data) {
        this->write_data = write_data;
    }

    /// 设置请求 body 外部来源，此选项将在请求完成后重置
    /// @param read_callback body 读取来源回调函数，返回读取大小，读取不完全则终止传输
    /// @param expect_total body 预计大小
    void setReadCallback(const ReadCallback &read_callback, size_t expect_total) {
        this->read_callback = read_callback;
        this->expect_total = expect_total;
    }

    /// 设置响应 body 外部目的地，此选项将在请求完成后重置
    /// @param write_callback 接收 body 目的地回调函数，返回写入大小，写入不完全则终止传输
    void setWriteCallback(const WriteCallback &write_callback) {
        this->write_callback = write_callback;
    }

    /// 重置 body 相关设置
    void reset() {
        expect_total = 0;
        read_data = &req->getBody();
        write_data = &resp->getBody();
        read_callback = nullptr;
        write_callback = nullptr;
    }

protected:
    Request::Ptr req = nullptr;
    Response::Ptr resp = nullptr;

    size_t expect_total = 0;

    io::PeekableStream *read_data = nullptr;
    io::OutputStream *write_data = nullptr;

    WriteCallback write_callback;
    ReadCallback read_callback;
};
} // namespace sese::net::http