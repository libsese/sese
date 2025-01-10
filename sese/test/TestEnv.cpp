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

#include <sese/system/Environment.h>
#include <sese/system/CommandLine.h>
#include <sese/log/Marco.h>
#include <sese/net/Socket.h>

#include <gtest/gtest.h>

using sese::system::CommandLine;
using sese::system::Environment;

TEST(TestEnv, Env) {
    SESE_INFO("build date: {}", Environment::getBuildDate());
    SESE_INFO("build time: {}", Environment::getBuildTime());
    SESE_INFO("build date time: {}", Environment::getBuildDateTime());
    SESE_INFO("endian type: {}", Environment::isLittleEndian() ? "Little" : "Big");
    SESE_INFO("os type: {}", Environment::getOperateSystemType());
    SESE_INFO("version: {}.{}.{}", Environment::getMajorVersion(), Environment::getMinorVersion(), Environment::getPatchVersion());
    SESE_INFO("git info: {}-{}", Environment::getRepoBranch(), Environment::getRepoHash());
}

TEST(TestEnv, CommandLine) {
    SESE_INFO("argc: {}", CommandLine::getArgc());
    for (int i = 0; i < CommandLine::getArgc(); ++i) {
        SESE_INFO("args[{}]={}", i, CommandLine::getArgv()[i]);
    }
}

TEST(TestEnv, Var) {
    sese::net::Socket(sese::net::Socket::Family::IPv4, sese::net::Socket::Type::TCP, 114514);
    auto message = sese::net::getNetworkErrorString();
    SESE_ERROR("{}", message.c_str());
    for (auto &&ch: message) {
        EXPECT_TRUE(static_cast<uint8_t>(ch) <= 127);
    }
    SESE_INFO("ENV{VCPKG_ROOT} is {}", sese::sys::Environment::getEnv("VCPKG_ROOT").c_str());
}