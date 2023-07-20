#include "sese/config/UniReader.h"
#include "sese/record/LogHelper.h"
#include "sese/util/Test.h"

using sese::record::LogHelper;
using sese::UniReader;
using sese::Test;

LogHelper helper("fUNI_READER");// NOLINT

int main() {
    UniReader reader;
    assert(helper, reader.create(PROJECT_PATH "/test/TestUniReader/data.txt"), -1);
    while (true) {
        auto line = reader.readLine();
        if (line.empty()) {
            break;
        } else {
            helper.info("%s", line.c_str());
        }
    }
    return 0;
}
