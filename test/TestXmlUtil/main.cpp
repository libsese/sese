#include <sese/config/xml/XmlUtil.h>
#include <sese/FileStream.h>
#include <sese/ConsoleOutputStream.h>
#include <sese/Test.h>
#include <sese/record/LogHelper.h>

using sese::record::LogHelper;
using sese::FileStream;
using sese::ConsoleOutputStream;
using sese::xml::XmlUtil;

LogHelper helper("fXML_UTIL"); // NOLINT

int main() {
    auto fileSteam = FileStream::create(PROJECT_PATH "/test/TestXmlUtil/data.xml", BINARY_READ_EXISTED);
    auto element = XmlUtil::deserialize(fileSteam, 5);
    assert(helper, element != nullptr, -1);

    auto subElement = std::make_shared<sese::xml::Element>("new-element");
    subElement->setAttribute("info", "from serialize");
    element->elements.push_back(subElement);

    auto saveFileStream = FileStream::create("out.xml", BINARY_WRITE_CREATE_TRUNC);
    assert(helper, saveFileStream->good(), -2);
    XmlUtil::serialize(element, saveFileStream);

    auto output = std::make_shared<ConsoleOutputStream>();
    XmlUtil::serialize(element, output);
    output->write("\n", 1);

    return 0;
}