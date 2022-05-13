#include "sese/Util.h"
#include "sese/record/FileAppender.h"
#include "sese/record/LogHelper.h"
#include "sese/record/SimpleFormatter.h"
#include "sese/Test.h"

sese::LogHelper helper("fLOGGER");// NOLINT

int main() {
    auto logger = sese::getLogger();
    auto formatter = std::make_shared<sese::SimpleFormatter>();
    auto fileStream = std::make_shared<sese::FileStream>();

    sese::Test::assert(helper, fileStream->open("hello.log", TEXT_WRITE_CREATE_TRUNC), -1);
    auto fileAppender = std::make_shared<sese::FileAppender>(fileStream, formatter);
    logger->addAppender(fileAppender);

    helper.debug("Hello 你好 こんにちは");
    helper.info("Hello 你好 こんにちは");
    helper.warn("Hello 你好 こんにちは");
    helper.error("Hello 你好 こんにちは");
    return 0;
}