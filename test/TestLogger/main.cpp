#include "sese/Util.h"
#include "sese/record/FileAppender.h"
#include "sese/record/LogHelper.h"
#include "sese/record/SimpleFormatter.h"
#include "sese/Test.h"

sese::record::LogHelper helper("fLOGGER");// NOLINT

int main() {
    auto logger = sese::record::getLogger();
    auto formatter = std::make_shared<sese::record::SimpleFormatter>();
    auto fileStream = sese::FileStream::create("hello.log", TEXT_WRITE_CREATE_TRUNC);

    assert(helper, nullptr != fileStream, -1);
    auto fileAppender = std::make_shared<sese::record::FileAppender>(fileStream, formatter);
    logger->addAppender(fileAppender);

    helper.debug("Hello 你好 こんにちは");
    helper.info("Hello 你好 こんにちは");
    helper.warn("Hello 你好 こんにちは");
    helper.error("Hello 你好 こんにちは");
    return 0;
}