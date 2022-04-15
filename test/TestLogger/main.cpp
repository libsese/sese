#include "Util.h"
#include "record/FileAppender.h"

#define FILTER_TEST_LOGGER "fLOGGER"

#ifdef _WIN32
#pragma warning(disable : 4819)
#endif

int main() {
    auto logger = sese::getLogger();
    auto formatter = std::make_shared<sese::SimpleFormatter>();
    auto fileAppender = std::make_shared<sese::FileAppender>("hello.log", formatter);
    logger->addAppender(fileAppender);
    ROOT_DEBUG(FILTER_TEST_LOGGER, "Hello 你好 こんにちは")
    ROOT_INFO(FILTER_TEST_LOGGER, "Hello 你好 こんにちは")
    ROOT_WARN(FILTER_TEST_LOGGER, "Hello 你好 こんにちは")
    ROOT_ERROR(FILTER_TEST_LOGGER, "Hello 你好 こんにちは")
    return 0;
}