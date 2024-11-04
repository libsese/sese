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