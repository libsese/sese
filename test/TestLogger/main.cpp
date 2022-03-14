#include "Util.h"
#include "record/FileAppender.h"

int main() {
    auto logger = sese::getLogger();
    auto formatter = std::make_shared<sese::SimpleFormatter>();
    auto fileAppender = std::make_shared<sese::FileAppender>("hello.log", formatter);
    logger->addAppender(fileAppender);
    ROOT_DEBUG("Hello")
    ROOT_INFO("Hello")
    ROOT_WARN("Hello")
    ROOT_ERROR("Hello")
    return 0;
}