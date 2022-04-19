#include "config/UniReader.h"
#include "record/LogHelper.h"

using sese::LogHelper;
using sese::UniReader;

LogHelper helper("fUNI_READER");// NOLINT

int main() {
    auto reader = std::make_shared<UniReader>(PROJECT_PATH "/test/TestUniReader/data.txt");
    while (true) {
        auto line = reader->readLine();
        if (line.empty()) {
            break;
        } else {
            helper.info("%s", line.c_str());
        }
    }
    return 0;
}
