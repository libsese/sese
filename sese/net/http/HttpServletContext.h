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

#pragma once

#include <sese/net/IPv6Address.h>
#include <sese/net/http/Request.h>
#include <sese/net/http/Response.h>

namespace sese::net::http {

class HttpServletContext {
public:
    HttpServletContext(Request &req, Response &resp, const IPAddress::Ptr &remote_address)
        : req(req), resp(resp), remote_address(remote_address) {
    }

    [[nodiscard]] auto getReq() const { return req; }
    [[nodiscard]] auto getResp() const { return resp; }
    [[nodiscard]] auto getRemoteAddress() const { return remote_address; }
    [[nodiscard]] io::InputStream *getInputStream() const { return &req.getBody(); }
    [[nodiscard]] io::OutputStream *getOutputStream() const { return &resp.getBody(); }

private:
    Request &req;
    Response &resp;
    const IPAddress::Ptr &remote_address;
};

} // namespace sese::net::http