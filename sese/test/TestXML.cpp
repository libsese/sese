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

#include "sese/config/xml/XmlUtil.h"
#include "sese/io/FileStream.h"
#include "sese/io/ConsoleOutputStream.h"
#include "sese/io/InputBufferWrapper.h"
#include "sese/record/LogHelper.h"

#include <gtest/gtest.h>

#include <filesystem>

TEST(TestXML, File) {
    auto file_steam = sese::io::FileStream::create(PROJECT_PATH "/sese/test/Data/data.xml", sese::io::FileStream::B_READ);
    auto element = sese::xml::XmlUtil::deserialize(file_steam, 5);
    ASSERT_NE(element, nullptr);

    for (decltype(auto) item: element->getElements()) {
        sese::record::LogHelper::i("name: %s, type: %s", item->getName().c_str(), item->getAttribute("type", "undef").c_str());
        for (decltype(auto) attr: item->getAttributes()) {
            sese::record::LogHelper::i("%s: %s", attr.first.c_str(), attr.second.c_str());
        }
        item->removeAttribute("count");
    }

    auto sub_element = std::make_shared<sese::xml::Element>("new-element");
    sub_element->setAttribute("info", "from serialize");
    element->addElement(sub_element);

    auto save_file_stream = sese::io::FileStream::create("out.xml", sese::io::FileStream::B_WRITE_TRUNC);
    ASSERT_NE(save_file_stream, nullptr);
    sese::xml::XmlUtil::serialize(element, save_file_stream);
    save_file_stream->close();
    std::filesystem::remove("out.xml");

    auto output = std::make_shared<sese::io::ConsoleOutputStream>();
    sese::xml::XmlUtil::serialize(element, output);
    output->write("\n", 1);
}

/// 不合法的元素结尾
TEST(TestXML, Error_0) {
    const char STR[] = {"<root>\n"
                        "    <item1>Hello</item2>\n"
                        "</root>"};
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    auto element = sese::xml::XmlUtil::deserialize(&input, 5);
    ASSERT_EQ(element, nullptr);
}

/// 不合法的元素结尾
TEST(TestXML, Error_1) {
    const char STR[] = {"<root>\n"
                        "    <item1>Hello<item1>\n"
                        "</root>"};
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    auto element = sese::xml::XmlUtil::deserialize(&input, 5);
    ASSERT_EQ(element, nullptr);
}

/// 不合法的元素结尾
TEST(TestXML, Error_2) {
    const char STR[] = {"<root>\n"
                        "    <item1>Hello</item1\n"
                        "</root>"};
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    auto element = sese::xml::XmlUtil::deserialize(&input, 5);
    ASSERT_EQ(element, nullptr);
}

/// 不合法的注释
TEST(TestXML, Error_3) {
    const char STR[] = {"<root>\n"
                        "    <!-- Content ->\n"
                        "</root>"};
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    auto element = sese::xml::XmlUtil::deserialize(&input, 5);
    ASSERT_EQ(element, nullptr);
}

/// 不合法的注释
TEST(TestXML, Error_4) {
    const char STR[] = {"<root>\n"
                        "    <!-- Content --\n"
                        "</root>"};
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    auto element = sese::xml::XmlUtil::deserialize(&input, 5);
    ASSERT_EQ(element, nullptr);
}

/// 错误子元素
TEST(TestXML, Error_5) {
    const char STR[] = {"<root>\n"
                        "    <item>\n"
                        "        <item>\n"
                        "            <item>Hello<item>\n"
                        "        </item>\n"
                        "    </item>\n"
                        "</root>"};
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    auto element = sese::xml::XmlUtil::deserialize(&input, 5);
    ASSERT_EQ(element, nullptr);
}

/// 根节点注释错误
TEST(TestXML, Error_6) {
    const char STR[] = {"<!-- CM ->\n"
                        "<root>\n"
                        "</root>"};
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    auto element = sese::xml::XmlUtil::deserialize(&input, 5);
    ASSERT_EQ(element, nullptr);
}

/// 节点深度超过限制
TEST(TestXML, Error_7) {
    const char STR[] = {"<root>\n"
                        "    <object>\n"
                        "    </object>\n"
                        "</root>"};
    auto input = sese::io::InputBufferWrapper(STR, sizeof(STR) - 1);
    auto element = sese::xml::XmlUtil::deserialize(&input, 1);
    ASSERT_EQ(element, nullptr);
}