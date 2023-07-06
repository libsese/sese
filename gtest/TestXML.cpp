#include "sese/config/xml/XmlUtil.h"
#include "sese/util/FileStream.h"
#include "sese/util/ConsoleOutputStream.h"
#include "sese/util/InputBufferWrapper.h"
#include "sese/record/LogHelper.h"

#include <gtest/gtest.h>

TEST(TestXML, File) {
    auto fileSteam = sese::FileStream::create(PROJECT_PATH "/test/TestXmlUtil/data.xml", BINARY_READ_EXISTED);
    auto element = sese::xml::XmlUtil::deserialize(fileSteam, 5);
    ASSERT_NE(element, nullptr);

    for (decltype(auto) item: element->getElements()) {
        sese::record::LogHelper::i("name: %s, type: %s", item->getName().c_str(), item->getAttribute("type", "undef").c_str());
        for (decltype(auto) attr: item->getAttributes()) {
            sese::record::LogHelper::i("%s: %s", attr.first.c_str(), attr.second.c_str());
        }
        item->removeAttribute("count");
    }

    auto subElement = std::make_shared<sese::xml::Element>("new-element");
    subElement->setAttribute("info", "from serialize");
    element->addElement(subElement);

    auto saveFileStream = sese::FileStream::create("out.xml", BINARY_WRITE_CREATE_TRUNC);
    ASSERT_NE(saveFileStream, nullptr);
    sese::xml::XmlUtil::serialize(element, saveFileStream);

    auto output = std::make_shared<sese::ConsoleOutputStream>();
    sese::xml::XmlUtil::serialize(element, output);
    output->write("\n", 1);
}

/// 不合法的元素结尾
TEST(TestXML, Error_0) {
    const char str[] = {"<root>\n"
                        "    <item1>Hello</item2>\n"
                        "</root>"};
    auto input = sese::InputBufferWrapper(str, sizeof(str) - 1);
    auto element = sese::xml::XmlUtil::deserialize(&input, 5);
    ASSERT_EQ(element, nullptr);
}

/// 不合法的元素结尾
TEST(TestXML, Error_1) {
    const char str[] = {"<root>\n"
                        "    <item1>Hello<item1>\n"
                        "</root>"};
    auto input = sese::InputBufferWrapper(str, sizeof(str) - 1);
    auto element = sese::xml::XmlUtil::deserialize(&input, 5);
    ASSERT_EQ(element, nullptr);
}

/// 不合法的元素结尾
TEST(TestXML, Error_2) {
    const char str[] = {"<root>\n"
                        "    <item1>Hello</item1\n"
                        "</root>"};
    auto input = sese::InputBufferWrapper(str, sizeof(str) - 1);
    auto element = sese::xml::XmlUtil::deserialize(&input, 5);
    ASSERT_EQ(element, nullptr);
}

/// 不合法的注释
TEST(TestXML, Error_3) {
    const char str[] = {"<root>\n"
                        "    <!-- Content ->\n"
                        "</root>"};
    auto input = sese::InputBufferWrapper(str, sizeof(str) - 1);
    auto element = sese::xml::XmlUtil::deserialize(&input, 5);
    ASSERT_EQ(element, nullptr);
}

/// 不合法的注释
TEST(TestXML, Error_4) {
    const char str[] = {"<root>\n"
                        "    <!-- Content --\n"
                        "</root>"};
    auto input = sese::InputBufferWrapper(str, sizeof(str) - 1);
    auto element = sese::xml::XmlUtil::deserialize(&input, 5);
    ASSERT_EQ(element, nullptr);
}

/// 错误子元素
TEST(TestXML, Error_5) {
    const char str[] = {"<root>\n"
                        "    <item>\n"
                        "        <item>\n"
                        "            <item>Hello<item>\n"
                        "        </item>\n"
                        "    </item>\n"
                        "</root>"};
    auto input = sese::InputBufferWrapper(str, sizeof(str) - 1);
    auto element = sese::xml::XmlUtil::deserialize(&input, 5);
    ASSERT_EQ(element, nullptr);
}