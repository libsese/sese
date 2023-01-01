#include "sese/util/Util.h"
#include "sese/record/FileAppender.h"
#include "sese/record/LogHelper.h"
#include "sese/record/SimpleFormatter.h"
#include "sese/util/Test.h"


int main() {
    sese::Initializer::getInitializer();
    sese::record::LogHelper helper("fLOGGER");// NOLINT

    auto logger = sese::record::getLogger();
    auto fileStream = sese::FileStream::create("hello.log", TEXT_WRITE_CREATE_TRUNC);

    assert(helper, nullptr != fileStream, -1);
    auto fileAppender = std::make_shared<sese::record::FileAppender>(fileStream);
    logger->addAppender(fileAppender);

    helper.debug("Hello");
    helper.info("Hello");
    helper.warn("Hello");
    helper.error("Hello");
    return 0;
}