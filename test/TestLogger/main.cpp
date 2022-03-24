#include "Util.h"
#include "record/FileAppender.h"

#define FILTER_TEST_LOGGER "fLOGGER"

int main() {
    auto logger = sese::getLogger();
    auto formatter = std::make_shared<sese::SimpleFormatter>();
    auto fileAppender = std::make_shared<sese::FileAppender>("hello.log", formatter);
    logger->addAppender(fileAppender);
    ROOT_DEBUG(FILTER_TEST_LOGGER, "Hello")
    ROOT_INFO(FILTER_TEST_LOGGER, "Hello")
    ROOT_WARN(FILTER_TEST_LOGGER, "Hello")
    ROOT_ERROR(FILTER_TEST_LOGGER, "Hello")
    return 0;
}