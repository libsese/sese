#include "config/UniReader.h"
#include "IOException.h"
#include "EncodingConvert.h"

sese::UniReader::UniReader(const std::string &fileName) {
    fileStream = std::make_shared<FileStream>(fileName, TEXT_READ_EXISTED);
    if(!fileStream->good()) throw sese::IOException(sese::getErrorString());
    reader = std::make_shared<StreamReader>(fileStream);
}

sese::UniReader::~UniReader() {
    fileStream->close();
}

std::string sese::UniReader::readLine() {
    return reader->readLine();
}