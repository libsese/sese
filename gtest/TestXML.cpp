#include "sese/config/xml/XmlUtil.h"
#include "sese/util/FileStream.h"
#include "sese/util/ConsoleOutputStream.h"

#include <gtest/gtest.h>

using sese::FileStream;
using sese::ConsoleOutputStream;
using sese::xml::XmlUtil;

TEST(TestXML, _0) {
    auto fileSteam = FileStream::create(PROJECT_PATH "/test/TestXmlUtil/data.xml", BINARY_READ_EXISTED);
    auto element = XmlUtil::deserialize(fileSteam, 5);
    ASSERT_NE(element, nullptr);

    auto subElement = std::make_shared<sese::xml::Element>("new-element");
    subElement->setAttribute("info", "from serialize");
    element->elements.push_back(subElement);

    auto saveFileStream = FileStream::create("out.xml", BINARY_WRITE_CREATE_TRUNC);
    ASSERT_TRUE(saveFileStream->good());
    XmlUtil::serialize(element, saveFileStream);

    auto output = std::make_shared<ConsoleOutputStream>();
    XmlUtil::serialize(element, output);
    output->write("\n", 1);
}