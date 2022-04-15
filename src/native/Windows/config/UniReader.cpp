#include "config/UniReader.h"
#include "IOException.h"
#include "EncodingConvert.h"

sese::UniReader::UniReader(const std::string &fileName) {
    fileStream = std::make_shared<FileStream>(fileName, TEXT_READ_EXISTED_W);
    if(!fileStream->good()) throw sese::IOException(sese::getErrorString());
    reader = std::make_shared<WStreamReader>(fileStream);
}

sese::UniReader::~UniReader() {
    fileStream->close();
}

std::string sese::UniReader::readLine() {
    std::wstring str = reader->readLine();
    return sese::EncodingConvert::toString(str);
}