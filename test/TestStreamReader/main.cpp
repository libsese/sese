#include "BaseStreamReader.h"
#include "FileStream.h"
#include "record/LogHelper.h"

using sese::FileStream;
using sese::LogHelper;
using sese::StreamReader;
using sese::WStreamReader;

LogHelper helper("fSTREAM_READER");// NOLINT

int main() {
    auto fileStream = std::make_shared<FileStream>(
            PROJECT_PATH "/test/TestStreamReader/data.txt",
            TEXT_READ_EXISTED);
    if (!fileStream->good()) {
        helper.info("%s", sese::getErrorString().c_str());
        return 0;
    }

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