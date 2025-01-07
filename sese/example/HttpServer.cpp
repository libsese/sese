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

#include <sese/service/http/HttpServer.h>
#include <sese/security/SSLContextBuilder.h>
#include <sese/util/Initializer.h>
#include <sese/record/Marco.h>
#include <sese/thread/Locker.h>
#include <sese/Util.h>

using sese::net::http::Controller;
using sese::net::http::Request;
using sese::net::http::Response;
using sese::net::http::HttpServletContext;

SESE_CTRL(MyController, std::mutex mutex{}; std::map<std::string, std::string> map{}; int times = 0) {
    SESE_INFO("LOADING MyController");
    SESE_URL(set, RequestType::GET, "/set?{name}&{id}") {
        sese::Locker locker(mutex);
        auto &req = ctx.getReq();
        auto name = req.getQueryArg("name");
        auto id = req.getQueryArg("id");
        map[id] = name;
        times += 1;
    };
    SESE_URL(get, RequestType::GET, "/get?{id}") {
        sese::Locker locker(mutex);
        auto &req = ctx.getReq();
        auto &resp = ctx.getResp();
        auto id = req.getQueryArg("id");
        auto iterator = map.find(id);
        std::string message;
        if (iterator == map.end()) {
            message = "cannot found name by id '" + id + "'\n";
        } else {
            message = "name = '" + iterator->second + "'\n";
        }
        resp.getBody().write(message.data(), message.length());
        times += 1;
    };
    SESE_URL(timers, RequestType::GET, "/times") {
        sese::Locker locker(mutex);
        auto &resp = ctx.getResp();
        times += 1;
        auto message = "timers = '" + std::to_string(this->times) + "'\n";
        resp.getBody().write(message.data(), message.length());
    };
    SESE_URL(say, RequestType::GET, "/say?<say>") {
        auto &req = ctx.getReq();
        auto words = req.get("say");
        auto message = "you say '" + words + "'\n";
        ctx.getOutputStream()->write(message.data(), message.length());
    };
    SESE_URL(ip, RequestType::GET, "/ip") {
        if (!ctx.getRemoteAddress()) {
            ctx.getResp().setCode(400);
            return;
        }
        auto message = "your ip is " + ctx.getRemoteAddress()->getAddress();
        ctx.getOutputStream()->write(message.data(), message.length());
    };
    SESE_INFO("LOADED");
}

int main(int argc, char **argv) {
    using sese::service::http::HttpServer;

    sese::initCore(argc, argv);
    auto ssl = sese::security::SSLContextBuilder::UniqueSSL4Server();
    ssl->importCertFile(PROJECT_PATH "/sese/test/Data/test-ca.crt");
    ssl->importPrivateKeyFile(PROJECT_PATH "/sese/test/Data/test-key.pem");

    auto server = HttpServer();
    server.setKeepalive(60);
    server.setName("HttpService");
    server.regMountPoint("/www", PROJECT_PATH "/build/html");
    server.regController<MyController>();
    server.regService(sese::net::IPv4Address::any(443), std::move(ssl->copy()));
    server.regService(sese::net::IPv4Address::any(80), nullptr);
    // The return value represents whether it needs to continue to be processed as a normal request, where false represents a direct return and response
    server.regFilter("/data1", [](Request &req, Response &resp) -> bool {
        SESE_INFO("filter /data1: {}", req.getUri());
        resp.setCode(404);
        return false;
    });
    // `true` stands for continued processing
    server.regFilter("/data2", [](Request &req, Response &resp) -> bool {
        SESE_INFO("filter /data2: {}", req.getUri());
        resp.set("myfilter", "data2");
        return true;
    });
    server.regFilter("/www", [](Request &req, Response &resp) -> bool {
        resp.set("msg", "continue");
        return true;
    });
    server.setConnectionCallback([](const sese::net::IPAddress::Ptr &address) -> bool {
        if (!address) {
            return false;
        }
        auto string = address->getAddress();
        if (string == "192.168.31.230") {
            SESE_INFO("connection from ban ip, closed");
            // disconnect
            return false;
        }
        SESE_INFO("address {}", string);
        // Continue processing
        return true;
    });

    server.startup();
    getchar();
    server.shutdown();
}