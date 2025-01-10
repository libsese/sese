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

#include <sese/log/Logger.h>
#include <sese/config/ConfigUtil.h>
#include <sese/config/xml/XmlUtil.h>
#include <sese/io/ConsoleOutputStream.h>
#include <sese/io/FileStream.h>
#include <gtest/gtest.h>

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(disable : 4996)
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

using sese::log::Logger;

TEST(TestConfig, Config) {
    auto file = sese::io::FileStream::create(PROJECT_PATH "/sese/test/Data/data.ini", sese::io::File::T_READ);
    ASSERT_TRUE(file != nullptr);

    auto config = sese::ConfigUtil::readFrom(file.get());
    ASSERT_TRUE(config != nullptr);

    auto default_section = config->getDefaultSection();
    ASSERT_TRUE(default_section != nullptr);
    auto default_name = default_section->getValueByKey("name", "unknown");
    Logger::info("[default] name = {}", default_name.c_str());

    auto server_section = config->getSectionByName("server");
    ASSERT_TRUE(server_section != nullptr);
    Logger::info("[server] address = {}", server_section->getValueByKey("address", "192.168.1.1").c_str());
    Logger::info("[server] port = {}", server_section->getValueByKey("port", "8080").c_str());

    auto client_section = config->getSectionByName("client");
    ASSERT_TRUE(client_section == nullptr);
}

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

TEST(TestConfig, Xml) {
    auto file = sese::io::FileStream::create(PROJECT_PATH "/sese/test/Data/data.xml", sese::io::File::B_READ);
    ASSERT_TRUE(file != nullptr);
    auto element = sese::xml::XmlUtil::deserialize(file, 5);
    ASSERT_TRUE(element != nullptr);

    auto sub_element = std::make_shared<sese::xml::Element>("new-element");
    sub_element->setAttribute("info", "from serialize");
    element->elements.push_back(sub_element);

    // auto saveFile = sese::FileStream::create("out.xml", BINARY_WRITE_CREATE_TRUNC);
    // ASSERT_TRUE(saveFile != nullptr);
    auto out = std::make_shared<sese::io::ConsoleOutputStream>();
    sese::xml::XmlUtil::serialize(element, out);
    out->write("\n", 1);
}