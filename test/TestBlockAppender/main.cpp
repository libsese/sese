#include <sese/record/Logger.h>
#include <sese/record/BlockAppender.h>
#include <sese/record/LogHelper.h>

using namespace sese::record;

LogHelper helper("BlockAppender");// NOLINT

int main() {
    auto appender = std::make_shared<BlockAppender>(40000);
    getLogger()->addAppender(appender);

    for (auto i = 0; i < 640; i++) {
        helper.info("No.%d log message", i);
    }

    return 0;
}