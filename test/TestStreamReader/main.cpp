#include "sese/BaseStreamReader.h"
#include "sese/FileStream.h"
#include "sese/record/LogHelper.h"
#include "sese/Test.h"

using sese::FileStream;
using sese::LogHelper;
using sese::StreamReader;
using sese::WStreamReader;
using sese::Test;

LogHelper helper("fSTREAM_READER");// NOLINT

int main() {
    auto fileStream = FileStream::create(PROJECT_PATH "/test/TestStreamReader/data.txt",TEXT_READ_EXISTED);
//    if (!fileStream->open(PROJECT_PATH "/test/TestStreamReader/data.txt",
//                          TEXT_READ_EXISTED)) {
//        helper.info("%s", sese::getErrorString().c_str());
//        return 0;
//    }
    assert(helper, nullptr != fileStream , -1);

    auto reader = std::make_shared<StreamReader>(fileStream);
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