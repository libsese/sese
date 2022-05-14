#include <sese/config/json/JsonUtil.h>
#include <sese/FileStream.h>
#include <sese/record/LogHelper.h>

using sese::json::JsonUtil;
using sese::FileStream;
using sese::LogHelper;

LogHelper helper("fJSON"); // NOLINT

int main() {
    auto fileStream = std::make_shared<FileStream>();
    fileStream->open(PROJECT_PATH"/test/TestJsonUtil/data.json", TEXT_READ_EXISTED);

    JsonUtil::Tokens tokens;
    JsonUtil::tokenizer(fileStream, tokens);
    auto object = JsonUtil::parser(tokens);
    return 0;
}