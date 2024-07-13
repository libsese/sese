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

#include <gtest/gtest.h>

#include "sese/net/http/Controller.h"
#include "sese/Log.h"

using sese::net::http::Servlet;
using sese::text::for_each;

// case: url query args
TEST(TestServlet, QueryArgs) {
    auto serv = Servlet(sese::net::http::RequestType::GET, "/api/v2/login?{user_name}&{}&{");
    EXPECT_EQ(sese::net::http::RequestType::GET, serv.getExpectType());
    EXPECT_EQ(1, serv.getExpectQueryArgs().size());
    EXPECT_EQ(0, serv.getExpectHeaders().size());
    SESE_INFO("querys: {}", for_each(serv.getExpectQueryArgs()));
}

// case: url header args
TEST(TestServlet, HeaderArgs) {
    auto serv = Servlet(sese::net::http::RequestType::POST, "/api/v2/login?<user_name>&<>&<");
    EXPECT_EQ(sese::net::http::RequestType::POST, serv.getExpectType());
    EXPECT_EQ(0, serv.getExpectQueryArgs().size());
    EXPECT_EQ(1, serv.getExpectHeaders().size());
    SESE_INFO("headers: {}", for_each(serv.getExpectHeaders()));
}

// case: no args
TEST(TestServlet, NoArgs) {
    auto serv = Servlet(sese::net::http::RequestType::POST, "/api/v2/login");
    EXPECT_EQ(sese::net::http::RequestType::POST, serv.getExpectType());
    EXPECT_EQ(0, serv.getExpectQueryArgs().size());
    EXPECT_EQ(0, serv.getExpectHeaders().size());
}

// case: no callback
TEST(TestServlet, NoCallback) {
    auto serv = Servlet(sese::net::http::RequestType::GET, "/api/v2/login");
    sese::net::http::Request req;
    sese::net::http::Response resp;
    serv.invoke(req, resp);
    EXPECT_EQ(500, resp.getCode());
}

// case: no expect type
TEST(TestServlet, NoExpectType) {
    auto serv = Servlet(sese::net::http::RequestType::POST, "/api/v2/login");
    serv = [](sese::net::http::Request &, sese::net::http::Response &) {};
    sese::net::http::Request req;
    sese::net::http::Response resp;
    serv.invoke(req, resp);
    EXPECT_EQ(403, resp.getCode());
}