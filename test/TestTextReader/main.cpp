#include <sese/text/TextReader.h>
#include <sese/record/LogHelper.h>
#include <sese/Test.h>

using sese::Test;
using sese::record::LogHelper;
using sese::text::TextReader;

LogHelper helper("fTEXT_READER");// NOLINT

int main() {
    TextReader reader;
    assert(helper, reader.open(PROJECT_PATH "/test/TestUniReader/data.txt"), -1);
    while (true) {
        auto line = reader.readLine();
        if (line.null()) {
            break;
        } else {
            if (line.empty()) {
                continue;
            } else {
                helper.info("%s", line.reverse().data());
            }
        }
    }
    return 0;
}