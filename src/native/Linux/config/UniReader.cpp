#include "sese/config/UniReader.h"
#include "sese/IOException.h"
#include "sese/convert/EncodingConverter.h"

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